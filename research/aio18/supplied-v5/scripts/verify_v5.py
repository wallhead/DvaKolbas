#!/usr/bin/env python3
from __future__ import annotations
import argparse, hashlib, json, struct, subprocess, sys
from pathlib import Path

EXPECTED_ARCHIVE='136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8'
PKG=Path(__file__).resolve().parent.parent
EVID=PKG/'evidence'/'v5'

def sha256(p:Path)->str:
    h=hashlib.sha256()
    with p.open('rb') as f:
        for chunk in iter(lambda:f.read(1<<20),b''):
            h.update(chunk)
    return h.hexdigest()

def pe_image(path:Path):
    b=path.read_bytes()
    if b[:2]!=b'MZ': raise ValueError(f'not PE: {path}')
    peoff=struct.unpack_from('<I',b,0x3c)[0]
    if b[peoff:peoff+4]!=b'PE\0\0': raise ValueError(f'bad PE signature: {path}')
    nsec=struct.unpack_from('<H',b,peoff+6)[0]
    optsz=struct.unpack_from('<H',b,peoff+20)[0]
    sec=peoff+24+optsz
    secs=[]
    for i in range(nsec):
        o=sec+i*40
        name=b[o:o+8].split(b'\0',1)[0].decode('ascii','replace')
        vsize,va,rawsz,raw=struct.unpack_from('<IIII',b,o+8)
        secs.append((name,va,max(vsize,rawsz),raw))
    return b,secs

def read_rva(path:Path,rva:int,n:int)->bytes:
    b,secs=pe_image(path)
    for _,va,span,raw in secs:
        if va<=rva<va+span:
            return b[raw+rva-va:raw+rva-va+n]
    raise ValueError(f'RVA {rva:#x} not mapped in {path}')

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('extracted_root',type=Path)
    ap.add_argument('--archive',type=Path)
    ap.add_argument('--output',type=Path)
    a=ap.parse_args()
    host=a.extracted_root/'SKSE'/'Plugins'/'SkyrimUpscaler.dll'
    pd=a.extracted_root/'UpscalerBasePlugin'/'PDPerfPlugin.dll'
    mods={'host':host,'pd':pd}
    lm=json.loads((EVID/'landmarks_v5.json').read_text())
    st=json.loads((EVID/'strings_v5.json').read_text())
    result={}
    result['module_hashes']={m:sha256(p) for m,p in mods.items()}
    result['module_hash_match']={m:result['module_hashes'][m]==lm['module_sha256'][m] for m in mods}
    if a.archive:
        result['archive_sha256']=sha256(a.archive)
        result['archive_hash_match']=result['archive_sha256']==EXPECTED_ARCHIVE
    else:
        result['archive_sha256']=None; result['archive_hash_match']=None
    bad=[]
    for rec in lm['records']:
        got=read_rva(mods[rec['module']],int(rec['rva'],16),int(rec['length'])).hex()
        if got!=rec['bytes']:
            bad.append({'rva':rec['rva'],'module':rec['module'],'expected':rec['bytes'],'got':got})
    result['landmark_count']=len(lm['records'])
    result['landmark_byte_mismatches']=bad
    result['landmarks_match']=not bad
    hb=host.read_bytes()
    sm=[]
    for rec in st['records']:
        ok=rec['text'].encode('ascii') in hb
        if not ok: sm.append(rec['text'])
    result['string_record_count']=len(st['records'])
    result['missing_strings']=sm
    result['strings_match']=not sm
    dis=list((EVID/'disassembly').glob('*.asm'))
    result['v5_disassembly_count']=len(dis)
    result['v5_disassembly_nonempty']=all(p.stat().st_size>0 for p in dis)
    result['metadata_files_present']=all((EVID/n).is_file() for n in ['findings_v5.json','pipeline_map_v5.json','strings_v5.json','landmarks_v5.json'])
    # Run transparent model tests.
    cp=subprocess.run([sys.executable,str(PKG/'scripts'/'reference_models_v5.py')],capture_output=True,text=True)
    result['reference_model_tests_exit_code']=cp.returncode
    result['reference_model_tests_pass']=cp.returncode==0
    result['reference_model_tests_output']=cp.stderr.strip().splitlines()[-3:] if cp.stderr else cp.stdout.strip().splitlines()[-3:]
    # Distribution-safety check: this evidence package should not redistribute target binaries/PDBs.
    forbidden=[]
    for p in PKG.rglob('*'):
        if p.is_file() and p.suffix.lower() in {'.dll','.exe','.pdb'}:
            forbidden.append(str(p.relative_to(PKG)))
    result['forbidden_binary_files']=forbidden
    result['no_target_binaries_in_package']=not forbidden
    bools=[v for k,v in result.items() if isinstance(v,bool)]
    result['all_boolean_checks_pass']=all(bools)
    out=json.dumps(result,indent=2)+'\n'
    if a.output: a.output.write_text(out)
    print(out,end='')
    return 0 if result['all_boolean_checks_pass'] and not bad and not sm and cp.returncode==0 else 1
if __name__=='__main__': raise SystemExit(main())
