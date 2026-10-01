#!/usr/bin/env python3
"""Small, read-only PE32+ inventory reader. No binaries are loaded/executed."""
import bisect, hashlib, json, pathlib, re, struct, sys, uuid
class PE:
 def __init__(self,path):
  self.path=pathlib.Path(path);self.data=self.path.read_bytes();d=self.data
  assert d[:2]==b'MZ';pe=self.u32(0x3c);assert d[pe:pe+4]==b'PE\0\0'
  self.machine,self.nsec,self.timestamp,_,_,nopt,_=struct.unpack_from('<HHIIIHH',d,pe+4)
  op=pe+24;assert self.u16(op)==0x20b
  self.base=self.u64(op+24);self.image_size=self.u32(op+56);self.entry=self.u32(op+16)
  self.dirs=[struct.unpack_from('<II',d,op+112+i*8) for i in range(min(16,self.u32(op+108)))]
  self.sections=[]
  for i in range(self.nsec):
   s=op+nopt+i*40;name=d[s:s+8].split(b'\0')[0].decode('ascii','replace')
   vs,va,rs,ro=struct.unpack_from('<IIII',d,s+8);ch=self.u32(s+36)
   self.sections.append({'name':name,'rva':va,'virtual_size':vs,'raw_size':rs,'raw_offset':ro,'flags':ch})
  self.exports=self.get_exports();self.imports=self.get_imports();self.delay_imports=self.get_imports(True)
  self.functions=[];self.invalid_functions=0
  rv,size=self.dirs[3]
  if rv:
   for i in range(0,size-11,12):
    b,e,u=struct.unpack('<III',self.read(rv+i,12))
    sec=self.section(b)
    if 0<b<e<=self.image_size and u<self.image_size and sec and sec['flags']&0x20000000:self.functions.append((b,e,u))
    else:self.invalid_functions+=1
  self.functions.sort();self.starts=[x[0] for x in self.functions]
 def u16(self,o):return struct.unpack_from('<H',self.data,o)[0]
 def u32(self,o):return struct.unpack_from('<I',self.data,o)[0]
 def u64(self,o):return struct.unpack_from('<Q',self.data,o)[0]
 def section(self,r):return next((s for s in self.sections if s['rva']<=r<s['rva']+max(s['virtual_size'],s['raw_size'])),None)
 def off(self,r):
  s=self.section(r)
  if s is None:
   if r<self.sections[0]['raw_offset']:return r
   raise ValueError(f'RVA {r:x} not mapped')
  delta=r-s['rva']
  if delta>=s['raw_size']:raise ValueError('RVA in zero fill')
  return s['raw_offset']+delta
 def rva(self,o):
  for s in self.sections:
   if s['raw_offset']<=o<s['raw_offset']+s['raw_size']:return s['rva']+o-s['raw_offset']
  return o
 def read(self,r,n):o=self.off(r);return self.data[o:o+n]
 def cstr(self,r):
  o=self.off(r);end=self.data.find(b'\0',o);return self.data[o:end].decode('utf8','replace')
 def ptr(self,r):return struct.unpack('<Q',self.read(r,8))[0]
 def get_exports(self):
  rv,size=self.dirs[0]
  if not rv:return []
  h=struct.unpack('<IIHHIIIIIII',self.read(rv,40));base,nfunc,nnames,ft,nt,ot=h[5:]
  names={}
  for i in range(nnames):
   nr=struct.unpack('<I',self.read(nt+i*4,4))[0];ordn=struct.unpack('<H',self.read(ot+i*2,2))[0];names[ordn]=self.cstr(nr)
  out=[]
  for i in range(nfunc):
   fr=struct.unpack('<I',self.read(ft+i*4,4))[0]
   if fr:out.append({'ordinal':base+i,'name':names.get(i,f'ordinal_{base+i}'),'rva':fr,'forwarder':self.cstr(fr) if rv<=fr<rv+size else None})
  return out
 def get_imports(self,delay=False):
  rv,size=self.dirs[13 if delay else 1]
  if not rv:return []
  out=[];entrysize=32 if delay else 20
  for j in range(0,size,entrysize):
   ds=struct.unpack('<'+'I'*(entrysize//4),self.read(rv+j,entrysize))
   if not any(ds):break
   if delay:
    attr,nm,_,iat,int_,_,_,_=ds
    if not attr&1:nm-=self.base;iat-=self.base;int_-=self.base
   else:int_,_,_,nm,iat=ds
   dll=self.cstr(nm);nt=int_ or iat
   for i in range(100000):
    v=self.ptr(nt+i*8)
    if v==0:break
    name=f'ordinal_{v&0xffff}' if v>>63 else self.cstr(v+2)
    out.append({'dll':dll,'name':name,'iat_rva':iat+i*8})
  return out
 def containing(self,r):
  i=bisect.bisect_right(self.starts,r)-1
  if i>=0 and self.functions[i][0]<=r<self.functions[i][1]:return self.functions[i]
  return None
 def codeview(self):
  rv,size=self.dirs[6];out=[]
  if not rv:return out
  for i in range(0,size-27,28):
   a=struct.unpack('<IIHHIIII',self.read(rv+i,28));typ,n,_,o=a[4:]
   if typ==2 and self.data[o:o+4]==b'RSDS':out.append({'guid':str(uuid.UUID(bytes_le=self.data[o+4:o+20])),'age':self.u32(o+20),'pdb':self.data[o+24:o+n].split(b'\0')[0].decode('utf8','replace')})
  return out
 def version(self):
  s=next((s for s in self.sections if s['name']=='.rsrc'),None)
  if not s:return None
  b=self.data[s['raw_offset']:s['raw_offset']+s['raw_size']];i=b.find(b'\xbd\x04\xef\xfe')
  if i<0:return None
  vs=struct.unpack_from('<6I',b,i)
  def v(a,b):return '.'.join(map(str,[a>>16,a&65535,b>>16,b&65535]))
  return {'file_version':v(vs[2],vs[3]),'product_version':v(vs[4],vs[5])}
 def strings(self,minlen=6):
  for enc,pat in [('ascii',rb'[\x20-\x7e]{'+str(minlen).encode()+rb',}'),('utf16le',rb'(?:[\x20-\x7e]\x00){'+str(minlen).encode()+rb',}')]:
   for m in re.finditer(pat,self.data):
    text=m.group().decode('ascii' if enc=='ascii' else 'utf-16le')
    yield {'rva':self.rva(m.start()),'file_offset':m.start(),'encoding':enc,'text':text}
 def inventory(self):return {'name':self.path.name,'sha256':hashlib.sha256(self.data).hexdigest(),'bytes':len(self.data),'image_base':self.base,'entry_rva':self.entry,'image_size':self.image_size,'sections':self.sections,'version':self.version(),'codeview':self.codeview(),'exports':self.exports,'imports':self.imports,'delay_imports':self.delay_imports,'valid_exception_ranges':len(self.functions),'invalid_exception_ranges':self.invalid_functions}
if __name__=='__main__':
 root=pathlib.Path(sys.argv[1]);out=pathlib.Path(sys.argv[2]);out.mkdir(parents=True,exist_ok=True)
 inventories=[]
 for file in sorted(root.rglob('*.dll')):
  p=PE(file);inventories.append(p.inventory());print(file.name,p.version(),len(p.functions),p.invalid_functions)
  if file.name in ['SkyrimUpscaler.dll','PDPerfPlugin.dll','amd_fidelityfx_framegeneration_dx12.dll','amd_fidelityfx_loader_dx12.dll','ffx_fsr3_x64.dll','ffx_fsr3upscaler_x64.dll']:
   (out/(file.stem+'_strings.json')).write_text(json.dumps(list(p.strings()),indent=2))
 (out/'pe_inventory.json').write_text(json.dumps(inventories,indent=2))
