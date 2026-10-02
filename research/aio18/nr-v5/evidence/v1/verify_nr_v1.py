#!/usr/bin/env python3
import argparse, hashlib, os, sys
from pathlib import Path
EXPECTED={
 "SkyrimUpscaler.dll":"5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81",
 "PDPerfPlugin.dll":"53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1",
 "nvngx_dlssnr.dll":"8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206",
}
REQUIRED_PD=[b"DLSSNR.Color",b"DLSSNR.Depth",b"DLSSNR.MVec",b"DLSSNR.Output",b"DLSSNR.UIAlpha",b"DLSSNR.UI",b"DLSSNR.Backbuffer",b"DLSSNR.Reset",b"DLSSNR.UICorrection",b"NGXNRHook"]
def sha(p):
 h=hashlib.sha256()
 with open(p,'rb') as f:
  for chunk in iter(lambda:f.read(1024*1024),b''): h.update(chunk)
 return h.hexdigest()
def main():
 ap=argparse.ArgumentParser();ap.add_argument('dir',type=Path);a=ap.parse_args(); ok=True
 for name,expected in EXPECTED.items():
  p=a.dir/name
  got=sha(p) if p.exists() else '<missing>'
  print(name,got)
  ok &= got==expected
 pd=(a.dir/'PDPerfPlugin.dll').read_bytes() if (a.dir/'PDPerfPlugin.dll').exists() else b''
 for s in REQUIRED_PD:
  present=s in pd; print(s.decode(),present); ok &= present
 return 0 if ok else 1
if __name__=='__main__': sys.exit(main())
