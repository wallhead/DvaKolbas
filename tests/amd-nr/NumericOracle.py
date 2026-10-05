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

def encode_e4m3_distance(words):
    """Mathematical nearest-distance oracle, independent of integer quantization."""
    words=np.asarray(words,dtype=np.uint32)
    codes=np.arange(127,dtype=np.uint32);e=(codes>>3)&15;m=codes&7
    finite=np.where(e==0,m.astype(np.float64)/512,np.ldexp(1+m.astype(np.float64)/8,e.astype(np.int32)-7))
    with np.errstate(all='ignore'):
        magnitude=(words&0x7fffffff).view(np.float32).astype(np.float64)
    safe=np.minimum(np.where(np.isnan(magnitude),0,magnitude),448)
    output=np.empty(len(words),dtype=np.uint32)
    for start in range(0,len(words),4096):
        distances=np.abs(safe[start:start+4096,None]-finite[None,:])
        chosen=distances.argmin(axis=1).astype(np.uint32)
        higher=np.minimum(chosen+1,126)
        rows=np.arange(len(chosen))
        tied=distances[rows,chosen]==distances[rows,higher]
        chosen+=((chosen&1)!=0)&tied
        output[start:start+len(chosen)]=chosen
    output|=(words>>24)&128
    output[(words&0x7fffffff)>0x7f800000]=0x7f
    return output

def e4m3_encode_fixtures():
    with np.errstate(all='ignore'):
        half=np.arange(65536,dtype=np.uint16).view(np.float16).astype(np.float32).view(np.uint32)
    neighbors=[]
    for c in range(126):
        def finite(code):
            e,m=code>>3,code&7
            return m/512 if e==0 else np.ldexp(1+m/8,e-7)
        a,b=np.float32(finite(c)),np.float32(finite(c+1));mid=np.float32((a+b)*0.5)
        for sign in [np.float32(1),np.float32(-1)]:
            neighbors.extend(np.float32(v*sign).view(np.uint32) for v in [np.nextafter(mid,a),mid,np.nextafter(mid,b)])
    edges=np.array([0,0x80000000,1,0x80000001,0x007fffff,0x807fffff,0x00800000,0x80800000,
                    0x1a996262,0x9a996262,0x3a800000,0xba800000,0x3a800001,0xba800001,
                    0x43e00000,0xc3e00000,0x7f800000,0xff800000,0x7f800001,0xff800001,0x7fc12345,0xffc12345],dtype=np.uint32)
    random=np.random.default_rng(20261005).integers(0,2**32,100000,dtype=np.uint32)
    words=np.concatenate([half,np.array(neighbors,dtype=np.uint32),edges,random])
    return words,encode_e4m3_distance(words)

class ComparisonTests(unittest.TestCase):
    def test_e4m3_literal_anchors(self):
        words=np.array([0,0x80000000,0x3f880000,0x3f980000,0x3a800000,0x3a800001,
                        0x43d80000,0x7f800000,0xff800000,0x7f800001,0xff800001],dtype=np.uint32)
        np.testing.assert_array_equal(encode_e4m3_distance(words),[0,0x80,0x38,0x3a,0,1,0x7e,0x7e,0xfe,0x7f,0x7f])
    def test_e4m3_deep_underflow(self):
        words=np.array([1,0x007fffff,0x00800000,0x1a996262],dtype=np.uint32)
        np.testing.assert_array_equal(encode_e4m3_distance(words),[0,0,0,0])
        np.testing.assert_array_equal(encode_e4m3_distance(words|0x80000000),[0x80]*4)
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
    words,encoded=e4m3_encode_fixtures()
    yield 'f32-to-e4m3',words,encoded

def verify(cpu,gpu,selection,directory):
    directory.mkdir(parents=True,exist_ok=True)
    report={'rng_seed':12345,'e4m3_rng_seed':20261005,'cases':[], 'unvalidated':['RDNA2 discrete','RDNA3 discrete','RDNA4 discrete'],
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
