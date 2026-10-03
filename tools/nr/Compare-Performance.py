"""Validate and summarize standalone NR transaction timings; never Skyrim FPS."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import statistics

CONTROLS=('scope','driverCoreSha256','adapterVendor','adapterDevice','adapterLuidLow','adapterLuidHigh',
          'width','height','placement','passes','preset','style','intensity','localTone','localStructure',
          'inputScale','resolve','hdr','encoding','scene','resetSchedule','debugLayer','timerPeriodMs','readbacks','warmup')
IDENTITIES=('runtimeSha256','compiledSourceSha256','instrumentation','nrEnabled')
BEFORE_PHASES=('prepareColor','prepareGuides','inputCopy','vendor','alpha','delivery','encode')


def match_kind(a,b):
    if any(k not in r or r[k] is None for r in (a,b) for k in (*CONTROLS,*IDENTITIES)):return 'unmatched'
    if any(a.get(k)!=b.get(k) for k in CONTROLS):return 'unmatched'
    runtime=a.get('runtimeSha256')!=b.get('runtimeSha256')
    source=a.get('compiledSourceSha256')!=b.get('compiledSourceSha256')
    instrumentation=a.get('instrumentation')!=b.get('instrumentation')
    enabled=a.get('nrEnabled')!=b.get('nrEnabled')
    if sum((runtime,source,instrumentation,enabled))>1:return 'unmatched'
    if runtime:return 'same-host-runtime-ab'
    if source:return 'same-model-host-change'
    if instrumentation:return 'instrumentation-ab'
    if enabled:return 'nr-on-off'
    return 'same-workload'


def issues(receipt):
    problems=[]
    if receipt.get('schema')!=1:problems.append('unknown schema')
    if any(k not in receipt or receipt[k] is None for k in (*CONTROLS,*IDENTITIES)):problems.append('missing control or identity')
    if receipt.get('rawInit')!=1:problems.append('initialization unconfirmed')
    if receipt.get('readbacks'):problems.append('readback capture is correctness-only')
    if not receipt.get('retired') or receipt.get('rawShutdown')!=1:problems.append('retirement unconfirmed')
    if receipt.get('failure'):problems.append('probe failure: '+str(receipt['failure']))
    if not receipt.get('timingComplete'):problems.append('timing incomplete')
    if any(receipt.get(k,0) for k in ('droppedFrames','droppedIntervals','gpuTimingDropped')):problems.append('dropped timing samples')
    samples=receipt.get('samples',[])
    if len(samples)!=receipt.get('requestedSamples'):problems.append('sample count mismatch')
    if len({s.get('source') for s in samples})!=len(samples):problems.append('duplicate source identity')
    for sample in samples:
        wall=sample.get('wallMilliseconds')
        if not isinstance(wall,(int,float)) or not math.isfinite(wall) or wall<0:problems.append('invalid wall timing');break
    for sample in samples:
        for group in ('cpuMilliseconds','waitMilliseconds','gpuMilliseconds'):
            values=sample.get(group,{})
            if not isinstance(values,dict) or any(v is not None and
                    (not isinstance(v,(int,float)) or not math.isfinite(v) or v<0) for v in values.values()):
                problems.append('invalid phase timing');break
        if receipt.get('instrumentation') and receipt.get('nrEnabled'):
            if any(sample.get('gpuMilliseconds',{}).get(p) is None for p in BEFORE_PHASES):problems.append('missing Before GPU timing')
            if any(sample.get(k)!=1 for k in ('descriptorCreations','submitted','completed')) or sample.get('waitCalls',0)<1:
                problems.append('incomplete source transaction')
    return problems


def distribution(values):
    ordered=sorted(v for v in values if isinstance(v,(int,float)) and math.isfinite(v) and v>=0)
    if not ordered:return {'count':0,'median':None,'p95':None,'p99':None}
    def percentile(p):
        position=(len(ordered)-1)*p;low=int(position);high=min(low+1,len(ordered)-1)
        return ordered[low]+(ordered[high]-ordered[low])*(position-low)
    return {'count':len(ordered),'median':statistics.median(ordered),'p95':percentile(.95),'p99':percentile(.99)}


def summarize(receipt):
    samples=receipt.get('samples',[])
    result={key:distribution([s.get(key) for s in samples]) for key in
            ('wallMilliseconds','blockMilliseconds','cpuUnionMilliseconds','waitCalls','blockingCalls','flushes','descriptorCreations')}
    for group in ('cpuMilliseconds','waitMilliseconds','gpuMilliseconds'):
        phases=sorted({p for s in samples for p in s.get(group,{})})
        result[group]={p:distribution([s.get(group,{}).get(p) for s in samples]) for p in phases}
    return result


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('receipts',type=Path,nargs='+')
    parser.add_argument('--output',type=Path)
    parser.add_argument('--require-clean',action='store_true')
    args=parser.parse_args()
    data=[json.loads(p.read_text(encoding='utf-8-sig')) for p in args.receipts]
    cases=[]
    for path,r in zip(args.receipts,data):
        invalid=issues(r)
        if args.require_clean and not r.get('cleanSource'):invalid.append('source was not clean at compilation')
        cases.append({'receipt':path.name,'profile':r.get('profile'),'nrEnabled':r.get('nrEnabled'),
                      'instrumentation':r.get('instrumentation'),'runtimeSha256':r.get('runtimeSha256'),
                      'receiptSha256':hashlib.sha256(path.read_bytes()).hexdigest(),
                      'identity':{k:v for k,v in r.items() if k!='samples'},
                      'cleanSource':r.get('cleanSource'),'issues':invalid,'summary':summarize(r)})
    comparisons=[]
    for i in range(len(data)):
        for j in range(i+1,len(data)):
            kind=match_kind(data[i],data[j])
            if kind=='unmatched' or cases[i]['issues'] or cases[j]['issues']:continue
            a=cases[i]['summary']['wallMilliseconds']['median'];b=cases[j]['summary']['wallMilliseconds']['median']
            comparisons.append({'a':args.receipts[i].name,'b':args.receipts[j].name,'kind':kind,'bMinusAMedianMilliseconds':b-a})
    report={'schema':1,'scope':'NR transaction timing; source/display FPS and AIO host performance not measured',
            'summarizerSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'cases':cases,'comparisons':comparisons}
    if args.output:
        args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
        for case in cases:print(f"{case['receipt']}: median NR transaction {case['summary']['wallMilliseconds']['median']} ms; issues={len(case['issues'])}")
    else:print(json.dumps(report,indent=2))
    return 1 if any(c['issues'] for c in cases) else 0


if __name__=='__main__':raise SystemExit(main())
