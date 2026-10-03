"""Benchmark validity checks; no GPU execution or fixture DLL loading."""
import importlib.util
from pathlib import Path
import unittest

script=Path(__file__).resolve().parents[2]/'tools/nr/Compare-Performance.py'
spec=importlib.util.spec_from_file_location('comparison',script)
comparison=importlib.util.module_from_spec(spec)
spec.loader.exec_module(comparison)


class Validity(unittest.TestCase):
    def receipt(self):
        return dict(schema=1,scope='standalone prepared Before NR transaction; no FSR/FG/Skyrim FPS',
                    sourceRevision='abc',compiledSourceSha256={'stage':'def'},runtimeSha256='modelA',
                    driverCoreSha256='core',adapterVendor=0x10de,adapterDevice=0x2702,
                    adapterLuidLow=1,adapterLuidHigh=0,width=2560,height=1440,placement='Before',
                    passes=1,preset=0,style=0,intensity=1,localTone=0,localStructure=1,
                    inputScale=1,resolve='Auto',hdr=False,encoding='Gamma22-to-linear-FP16',
                    scene='static',resetSchedule='first',debugLayer=False,timerPeriodMs=1,warmup=120,
                    readbacks=False,instrumentation=True,nrEnabled=True,retired=True,rawInit=1,
                    rawShutdown=1,timingComplete=True,cleanSource=True,requestedSamples=3,
                    failure='',droppedFrames=0,droppedIntervals=0,gpuTimingDropped=0,
                    samples=[dict(source=i+1,wallMilliseconds=float(i+1),waitCalls=6,descriptorCreations=1,submitted=1,completed=1,
                                  gpuMilliseconds={p:float(i) for p in ('prepareColor','prepareGuides','inputCopy','vendor','alpha','delivery','encode')}) for i in range(3)])

    def test_unmatched_settings_and_model(self):
        a=self.receipt();b=self.receipt()
        self.assertEqual(comparison.match_kind(a,b),'same-workload')
        b['timerPeriodMs']=0;self.assertEqual(comparison.match_kind(a,b),'unmatched')
        b=self.receipt();b['runtimeSha256']='modelB'
        self.assertEqual(comparison.match_kind(a,b),'same-host-runtime-ab')
        b['width']=1920;self.assertEqual(comparison.match_kind(a,b),'unmatched')
        b=self.receipt();b['placement']='After';self.assertEqual(comparison.match_kind(a,b),'unmatched')
        b=self.receipt();b['localTone']=1;self.assertEqual(comparison.match_kind(a,b),'unmatched')

    def test_correctness_capture_cannot_be_timing_baseline(self):
        a=self.receipt();a['readbacks']=True
        self.assertIn('readback', ' '.join(comparison.issues(a)))
        a=self.receipt();a['retired']=False
        self.assertIn('retirement', ' '.join(comparison.issues(a)))

    def test_missing_samples_or_timing_not_silently_dropped(self):
        a=self.receipt();a['samples'].pop()
        self.assertIn('sample count', ' '.join(comparison.issues(a)))
        a=self.receipt();a['gpuTimingDropped']=1
        self.assertIn('dropped', ' '.join(comparison.issues(a)))

    def test_percentiles_not_fps(self):
        summary=comparison.summarize(self.receipt())
        self.assertEqual(summary['wallMilliseconds']['median'],2)
        self.assertAlmostEqual(summary['wallMilliseconds']['p95'],2.9)
        self.assertAlmostEqual(summary['wallMilliseconds']['p99'],2.98)
        self.assertNotIn('fps',summary)

    def test_missing_controls_and_invalid_phase_values_are_rejected(self):
        a=self.receipt();b=self.receipt()
        del a['width'];del b['width']
        self.assertEqual(comparison.match_kind(a,b),'unmatched')
        self.assertIn('missing control', ' '.join(comparison.issues(a)))
        for value in (float('nan'),float('inf'),-1,'invalid'):
            a=self.receipt();a['samples'][0]['gpuMilliseconds']['vendor']=value
            self.assertIn('invalid phase', ' '.join(comparison.issues(a)))

    def test_init_and_warmup_are_required_controls(self):
        a=self.receipt();a['rawInit']=0
        self.assertIn('initialization', ' '.join(comparison.issues(a)))
        a=self.receipt();b=self.receipt();a['warmup']=120;b['warmup']=0
        self.assertEqual(comparison.match_kind(a,b),'unmatched')

    def test_complete_flag_cannot_hide_missing_gpu_phase_or_retirement(self):
        a=self.receipt();a['samples'][0]['gpuMilliseconds']['encode']=None
        self.assertIn('missing Before GPU timing', ' '.join(comparison.issues(a)))
        a=self.receipt();a['samples'][0]['completed']=0
        self.assertIn('incomplete source transaction', ' '.join(comparison.issues(a)))


if __name__=='__main__':unittest.main()
