import pathlib, struct, subprocess, sys, tempfile
exe=sys.argv[1]
def run(args,code):
    p=subprocess.run([exe,*map(str,args)],capture_output=True,text=True)
    assert p.returncode==code,(args,p.returncode,p.stdout,p.stderr)
    return p.stdout
with tempfile.TemporaryDirectory(prefix='TRP AMD NR ') as folder:
    d=pathlib.Path(folder); source=d/'input words.bin'; target=d/'output words.bin'
    run(['--help'],0)
    for args in [[],['--foo'],['--width'],['--help','--help'],['--width','1','--width','2']]:run(args,2)
    for width in ['-1','0','4294967296','2147483647','1x']:
        run(['--weights',source,'--width',width,'--height','1'],2)
    for mode in ['bad','-1']:
        run(['--weights',source,'--width','1','--height','1','--mode',mode],2)
    run(['--weights',source,'--width','1','--height','1'],2)
    source.write_bytes(b'invalid archive')
    run(['--weights',source,'--width','1','--height','1'],2)
    # Complete, structurally valid one-record archive.
    source.write_bytes(b'DLSSNRW1'+struct.pack('<II',1,34)+b'\x01x'+struct.pack('<QQ',0,1)+b'\x00')
    text=run(['--weights',source,'--width','1920','--height','1080','--list'],3)
    assert 'UnsupportedArchive' in text and 'inference=unavailable' in text
    source.write_bytes(struct.pack('<III',0x3f800000,0x80000000,0x7f800001))
    fmt=['--format','f32-to-f16','--input',source,'--output',target]
    run(fmt,0);assert target.read_bytes()==struct.pack('<III',0x3c00,0x8000,0x7e00)
    original=source.read_bytes()
    run(['--format','f32-to-f16','--input',source,'--output',source],2)
    alias=d/'alias.bin';alias.hardlink_to(source)
    run(['--format','f32-to-f16','--input',source,'--output',alias],2)
    assert source.read_bytes()==original
    run(['--format','f32-to-f16','--input',source,'--output',d/'missing'/'out.bin'],2)
    source.write_bytes(b'123');run(fmt,2)
    source.write_bytes(b'');run(fmt,0);assert target.read_bytes()==b''
    run(fmt+['--width','1'],2)
    run(['--format','bad','--input',source,'--output',target],2)
print('PASS: AMD NR inspector')
