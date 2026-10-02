#!/usr/bin/env python3
import argparse, hashlib, json, struct, sys
from pathlib import Path
EXPECTED = {
    'SkyrimUpscaler.dll':'5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81',
    'PDPerfPlugin.dll':'53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1',
    'nvngx_dlssnr.dll':'8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206',
}
ARCHIVE_SHA='136cbacda0d75373ad47e07c6017924edaaddbda063abaeb53f7e30734076af8'
LANDMARKS = {
 'SkyrimUpscaler.dll': {
  0x2A25E0:'ff157a061d00',       # EvaluateDLSSNR delay-IAT call
  0x2A2492:'c5fa104310',         # host +0x10 -> external MVecScaleX
  0x2A24F8:'4c89652844',         # external +0x108 zero/qword state
  0x2A2513:'c6453001',           # route selector set
  0x2A2524:'c5f81083cc020000',   # resolve scalar block source
 },
 'PDPerfPlugin.dll': {
  0x9AF8C:'4d8b01',             # internal Color load
  0x9AFA7:'e8f4860600',         # SetD3d12Resource(Color)
  0x9AFF8:'4c8b4728',           # internal UI resource load
  0x9B44C:'448b8704010000',     # internal UICorrection load for SetI
  0x9FDB4:'4180b80301000000',   # external private correction flag read
  0x9FF96:'41898a04010000',     # internal UICorrection = 0
  0x9E643:'33f6',               # rsi = 0
  0x9E7EB:'4889742478',         # core internal UI slot = 0
  0x9E7F0:'4889b42480000000',   # core internal UIAlpha slot = 0
  0xA0957:'48837f2000',         # immediate route requires external Output (+0x20)
  0xA09F6:'488b4708',           # external Color
  0xA0A02:'4c8b6f20',          # external Output
  0xA0A1A:'488b7f48',          # external BidirectionalDistortionField
  0x9F1C1:'4180bd0301000000',   # private UI correction flag checked by wrapper
 },
}
PD_STRINGS=[
 'DLSSNR.Color','DLSSNR.MVec','DLSSNR.Depth','DLSSNR.Output','DLSSNR.ControlMask','DLSSNR.UI','DLSSNR.UIAlpha',
 'DLSSNR.Backbuffer','DLSSNR.BidirectionalDistortionField','DLSSNR.MVecScaleX','DLSSNR.MVecScaleY','DLSSNR.Intensity',
 'DLSSNR.LocalToneStrength','DLSSNR.LocalStructureStrength','DLSSNR.SkinStructureStrength','DLSSNR.UseAutoMask','DLSSNR.Style',
 'DLSSNR.Reset','DLSSNR.DepthInverted','DLSSNR.Enabled','DLSSNR.UICorrection','DLSS.Indicator.Invert.X.Axis','DLSS.Indicator.Invert.Y.Axis',
 'DLSSNR debug: UIAlpha overwrite requested but uiAlpha is null','DLSSNR: immediate D3D12 evaluate missing output.'
]
HOST_STRINGS=[
 'mDLSSNRBeforeUpscaling','mDLSSNRPreset','mDLSSNRStyle','mDLSSNRIntensity','mDLSSNRLocalToneStrength',
 'mDLSSNRLocalStructureStrength','mDLSSNRSkinStructureStrength','mDLSSNRUseAutoMask','mDLSSNRUICorrection',
 'mDLSSNRResolveMethod','mDLSSNRInputResolutionScale','mDLSSNRTransferStrength','mDLSSNRColourStrength',
 'mDLSSNRMaxRatio','mDLSSNRWhitePoint','mDLSSNRColorIsHdr','mDLSSNRPass'
]
def sha(p):
 h=hashlib.sha256()
 with open(p,'rb') as f:
  for c in iter(lambda:f.read(1024*1024),b''): h.update(c)
 return h.hexdigest()
def read_rva(path,rva,n):
 data=path.read_bytes(); pe=struct.unpack_from('<I',data,0x3c)[0]
 if data[pe:pe+4] != b'PE\0\0': raise ValueError('bad PE')
 ns=struct.unpack_from('<H',data,pe+6)[0]; so=struct.unpack_from('<H',data,pe+20)[0]; st=pe+24+so
 for i in range(ns):
  o=st+i*40; vs,va,rawsz,raw=struct.unpack_from('<IIII',data,o+8)
  if va <= rva < va+max(vs,rawsz):
   off=raw+(rva-va); return data[off:off+n]
 raise ValueError(f'RVA not mapped {rva:#x}')
def check(cond,msg,out):
 out.append({'check':msg,'passed':bool(cond)})
 return bool(cond)
def main():
 ap=argparse.ArgumentParser(); ap.add_argument('module_dir',type=Path); ap.add_argument('--archive',type=Path); ap.add_argument('--evidence',type=Path,default=Path(__file__).parent)
 a=ap.parse_args(); out=[]; ok=True
 for name,exp in EXPECTED.items():
  p=a.module_dir/name; got=sha(p) if p.exists() else '<missing>'; ok &= check(got==exp,f'{name} SHA256 = {got}',out)
 if a.archive:
  got=sha(a.archive) if a.archive.exists() else '<missing>'; ok &= check(got==ARCHIVE_SHA,f'archive SHA256 = {got}',out)
 for name,items in LANDMARKS.items():
  p=a.module_dir/name
  for rva,hx in items.items():
   got=read_rva(p,rva,len(bytes.fromhex(hx))).hex(); ok &= check(got==hx,f'{name}+{rva:#x} bytes {got}',out)
 pd=(a.module_dir/'PDPerfPlugin.dll').read_bytes(); host=(a.module_dir/'SkyrimUpscaler.dll').read_bytes()
 for s in PD_STRINGS: ok &= check(s.encode() in pd,f'PD string: {s}',out)
 for s in HOST_STRINGS: ok &= check(s.encode() in host,f'host string: {s}',out)
 ext=json.loads((a.evidence/'external_payload_map.json').read_text()); ngx=json.loads((a.evidence/'ngx_parameter_map.json').read_text())
 ok &= check(ext.get('size_bytes')==0x138,'external payload size 0x138',out)
 ok &= check(ngx.get('size_bytes')==0x110,'internal NGX block size 0x110',out)
 er={x['role']:x['offset'] for x in ext['fields']}; ir={x['role']:x['offset'] for x in ngx['fields']}
 for role,off in {'Color':'0x008','MVec':'0x010','Depth':'0x018','Output':'0x020','UI':'0x030','UIAlpha':'0x038','Backbuffer':'0x040','BidirectionalDistortionField':'0x048','PrivateUICorrectionRequest':'0x103','ResolveMethod':'0x114','PassCountOrPassMode':'0x130'}.items():
  ok &= check(er.get(role)==off,f'external map {role}={off}',out)
 for role,off in {'Color':'0x000','UI':'0x028','UIAlpha':'0x030','Backbuffer':'0x038','MVecScaleX':'0x0D8','UICorrection':'0x104','IndicatorInvertY':'0x10C'}.items():
  ok &= check(ir.get(role)==off,f'internal map {role}={off}',out)
 ov=ngx.get('observed_core_overrides',{})
 ok &= check(ov.get('UI')=='null' and ov.get('UIAlpha')=='null' and ov.get('UICorrection')==0,'core UI/UIAlpha null and NGX UICorrection=0',out)
 result={'result':'PASS' if ok else 'FAIL','checks':len(out),'failures':sum(not x['passed'] for x in out),'details':out}
 print(json.dumps(result,indent=2)); return 0 if ok else 1
if __name__=='__main__': sys.exit(main())
