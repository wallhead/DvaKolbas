import pathlib, struct, subprocess, sys, tempfile
exe=sys.argv[1]
def run(args,code):
    p=subprocess.run([exe,*map(str,args)],capture_output=True,text=True)
    assert p.returncode==code,(args,p.returncode,p.stdout,p.stderr)
    return p.stdout
with tempfile.TemporaryDirectory(prefix='TRP AMD NR GPU ') as folder:
    d=pathlib.Path(folder); source=d/'input words.bin'; target=d/'output words.bin'
    run(['--help'],0)
    for args in [[],['--warp'],['--adapter-luid'],['--foo'],['--warp','--warp'],['--help','--warp']]:run(args,2)
    source.write_bytes(struct.pack('<IIII',0,1,128,255))
    fmt=['--format','e4m3-to-f16','--input',source,'--output',target]
    for luid in ['ffffffff:ffffffff','x:0','0:','0:100000000','-1:0','0:0:0']:
        run(['--adapter-luid',luid,*fmt],2)
    run(fmt,2);run(['--warp','--adapter-luid','0:0',*fmt],2)
    run(['--warp',*fmt],0);assert target.read_bytes()==struct.pack('<IIII',0,0x1800,0x8000,0x7e00)
    original=source.read_bytes()
    run(['--warp','--format','e4m3-to-f16','--input',source,'--output',source],2)
    alias=d/'alias.bin';alias.hardlink_to(source)
    run(['--warp','--format','e4m3-to-f16','--input',source,'--output',alias],2)
    assert source.read_bytes()==original
    source.write_bytes(b'123');run(['--warp',*fmt],2)
    source.write_bytes(b'');run(['--warp',*fmt],0);assert target.read_bytes()==b''
    run(['--warp','--format','bad','--input',source,'--output',target],2)
    run(['--warp','--format','e4m3-to-f16','--input',source,'--output',d/'absent'/'out.bin'],2)
    source.write_bytes(struct.pack('<IIIIII',0,0x80000000,0x3f800000,0x7f800000,0xff800001,0x9a996262))
    fp8=['--format','f32-to-e4m3','--input',source,'--output',target]
    run(['--warp',*fp8],0);assert target.read_bytes()==struct.pack('<IIIIII',0,0x80,0x38,0x7e,0x7f,0x80)
    original=source.read_bytes();previous=target.read_bytes()
    for output in [source,alias]:
        run(['--warp','--format','f32-to-e4m3','--input',source,'--output',output],2)
        assert source.read_bytes()==original and target.read_bytes()==previous
    run(['--warp','--format','f32-to-e4m3','--input',source,'--output',d/'absent'/'out.bin'],2)
    assert target.read_bytes()==previous
    source.write_bytes(b'123');run(['--warp',*fp8],2);assert target.read_bytes()==previous
    source.write_bytes(b'');run(['--warp',*fp8],0);assert target.read_bytes()==b''
print('PASS: explicit GPU selection and file conversion')
