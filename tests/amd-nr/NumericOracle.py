"""Independent storage-bit oracle. Requires NumPy; never loads the reference DLL."""
import argparse
import json
import pathlib
import subprocess
import tempfile
import unittest
import numpy as np

def compare_words(actual: bytes, expected: bytes) -> None:
    if len(actual)%4 or len(expected)%4:
        raise ValueError('malformed storage-word byte count')
    if len(actual)!=len(expected):
        raise ValueError(f'output length {len(actual)} != expected {len(expected)}')
    a=np.frombuffer(actual,dtype='<u4');e=np.frombuffer(expected,dtype='<u4')
    differences=np.flatnonzero(a!=e)
    if len(differences):
        first=int(differences[0])
        raise ValueError(f'{len(differences)} mismatches; word {first}: {a[first]:08x} != {e[first]:08x}')

def read_output(path):
    try:
        return pathlib.Path(path).read_bytes()
    except OSError as error:
        raise ValueError('output missing/unreadable') from error

class ComparisonTests(unittest.TestCase):
    def test_complete(self):
        compare_words(b'\x00\x00\x00\x00',b'\x00\x00\x00\x00')
        compare_words(b'',b'')
    def test_malformed(self):
        for actual,expected in [(b'x',b'x'),(b'123',b'0000'),(b'0000',b'123')]:
            with self.assertRaises(ValueError): compare_words(actual,expected)
    def test_truncated_or_missing_words(self):
        for actual in [b'',b'0000',b'000000000000']:
            with self.assertRaises(ValueError): compare_words(actual,b'00000000')
    def test_signed_zero(self):
        with self.assertRaises(ValueError): compare_words(b'\x00\x80\x00\x00',b'\x00\x00\x00\x00')
    def test_missing_output(self):
        with tempfile.TemporaryDirectory() as d:
            with self.assertRaises(ValueError): read_output(pathlib.Path(d)/'missing')

def fixtures():
    rng=np.random.default_rng(12345)
    random=rng.integers(0,2**32,50000,dtype=np.uint32)
    boundaries=[]
    for sign in [0,0x80000000]:
        for exponent in [0,1,101,102,103,112,113,126,127,142,143,254,255]:
            for mantissa in [0,1,0xfff,0x1000,0x1001,0x1fff,0x2000,0x2001,0x3fffff,0x7fdfff,0x7fe000,0x7fefff,0x7ff000,0x7ff001,0x7fffff]:
                center=sign|(exponent<<23)|mantissa
                boundaries.extend((center+offset)&0xffffffff for offset in range(-2,3))
    words=np.concatenate([random,np.array(boundaries,dtype=np.uint32)])
    with np.errstate(all='ignore'):
        expected=words.view(np.float32).astype(np.float16).view(np.uint16).astype(np.uint32)
    # Explicit, independently stated NaN contract: sign, retained payload, quiet bit.
    nan=((words&0x7f800000)==0x7f800000)&((words&0x7fffff)!=0)
    expected[nan]=((words[nan]>>16)&0x8000)|0x7c00|((words[nan]&0x7fffff)>>13)|0x200
    yield 'f32-to-f16',words,expected
    half=np.arange(65536,dtype=np.uint16)
    with np.errstate(all='ignore'):
        expanded=half.view(np.float16).astype(np.float32).view(np.uint32)
    # NumPy preserves signalling half payloads on this conversion. Assert the policy.
    nan=((half&0x7c00)==0x7c00)&((half&1023)!=0)
    required=((half[nan].astype(np.uint32)&0x8000)<<16)|0x7f800000|((half[nan].astype(np.uint32)&1023)<<13)
    if not np.array_equal(expanded[nan],required):
        raise ValueError('NumPy half expansion NaN policy differs; oracle environment unsupported')
    yield 'f16-to-f32',half.astype(np.uint32)|0xffff0000,expanded
    code=np.arange(256,dtype=np.uint32);e=(code>>3)&15;m=code&7
    value=np.where(e==0,m.astype(np.float64)/512,np.ldexp(1+m.astype(np.float64)/8,e.astype(np.int32)-7))
    value=np.copysign(value,np.where(code&128,-1.,1.))
    fp8=value.astype(np.float16).view(np.uint16).astype(np.uint32)
    fp8[(code&127)==127]=0x7e00
    yield 'e4m3-to-f16',code|0xffff0000,fp8

def verify(cpu,gpu,selection,directory):
    directory.mkdir(parents=True,exist_ok=True)
    report={'rng_seed':12345,'cases':[], 'unvalidated':['RDNA2 discrete','RDNA3 discrete','RDNA4 discrete'],
            'scope':'Storage conversions only; no inference or numerical accumulation acceptance.'}
    for operation,words,expected in fixtures():
        source=directory/(operation+'.input.bin')
        reference=directory/(operation+'.expected.bin')
        source.write_bytes(words.astype('<u4').tobytes());expected_bytes=expected.astype('<u4').tobytes();reference.write_bytes(expected_bytes)
        for name,exe,adapter in [('cpu',cpu,[])]+([('gpu',gpu,selection)] if gpu else []):
            output=directory/(operation+'.'+name+'.bin')
            # Remove our own previous result so a failed run cannot reuse a stale output.
            output.unlink(missing_ok=True)
            result=subprocess.run([str(exe),*adapter,'--format',operation,'--input',str(source),'--output',str(output)],capture_output=True,text=True)
            case={'engine':name,'operation':operation,'words':len(words),'returncode':result.returncode,'device_log':result.stdout,'stderr':result.stderr}
            report['cases'].append(case)
            try:
                if result.returncode: raise ValueError(f'{name} conversion failed: {result.stderr}')
                compare_words(read_output(output),expected_bytes);case['mismatches']=0
            except ValueError as error:
                case['failure']=str(error);(directory/'report.json').write_text(json.dumps(report,indent=2));raise
            print(f'{name} {operation}: {len(words)} words, 0 mismatches')
    (directory/'report.json').write_text(json.dumps(report,indent=2))

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--self-test',action='store_true');parser.add_argument('--cpu',type=pathlib.Path)
    parser.add_argument('--gpu',type=pathlib.Path);parser.add_argument('--work-dir',type=pathlib.Path)
    adapter=parser.add_mutually_exclusive_group();adapter.add_argument('--warp',action='store_true');adapter.add_argument('--adapter-luid')
    args=parser.parse_args()
    if args.self_test:
        if args.cpu or args.gpu or args.work_dir or args.warp or args.adapter_luid:parser.error('--self-test takes no other options')
        suite=unittest.defaultTestLoader.loadTestsFromTestCase(ComparisonTests)
        return 0 if unittest.TextTestRunner().run(suite).wasSuccessful() else 1
    if not args.cpu or not args.work_dir:parser.error('--cpu and --work-dir required')
    if bool(args.gpu)!=bool(args.warp or args.adapter_luid):parser.error('GPU requires exactly one explicit adapter choice')
    selection=['--warp'] if args.warp else ['--adapter-luid',args.adapter_luid] if args.adapter_luid else []
    try:verify(args.cpu.resolve(),args.gpu.resolve() if args.gpu else None,selection,args.work_dir.resolve())
    except (ValueError,OSError) as error:print(f'FAIL: {error}');return 1
    return 0

if __name__=='__main__':
    raise SystemExit(main())
