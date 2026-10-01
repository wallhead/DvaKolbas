#!/usr/bin/env python3
"""Index objdump x64 disassembly, PE exception ranges and RIP-relative references."""
import bisect,json,pathlib,re,struct,sys
from pe_index import PE
class Disasm:
 def __init__(self,pe,asm,strings=None,symbols=None):
  self.pe=pe;self.ins=[];self.byref={};self.byaddr={};self.names={x['rva']:x['name'] for x in pe.exports};self.strings={}
  if symbols:
   for s in json.load(open(symbols)):
    if 'rva' in s and not s['name'].startswith('?'):self.names[s['rva']]=s['name']
  if strings:self.strings={s['rva']:s['text'] for s in json.load(open(strings)) if len(s['text'])<600}
  self.iat={x['iat_rva']:x['dll']+'!'+x['name'] for x in pe.imports+pe.delay_imports}
  for line in open(asm):
   m=re.match(r'^\s*([0-9a-f]+):\s+(\S+)\s*(.*)$',line)
   if not m:continue
   addr=int(m[1],16)-pe.base;mn=m[2];op=m[3];refs=[]
   # Direct branches and objdump's RIP-relative resolved comment.
   if mn=='call' or mn.startswith('j'):
    v=re.match(r'(?:0x)?([0-9a-f]{8,16})(?:\s|$)',op)
    if v:refs.append(int(v[1],16)-pe.base)
   v=re.search(r'#\s*(?:0x)?([0-9a-f]{8,16})',op)
   if v:refs.append(int(v[1],16)-pe.base)
   rec={'rva':addr,'mnemonic':mn,'operands':op,'refs':list(dict.fromkeys(refs))}
   self.ins.append(rec);self.byaddr[addr]=rec
   for r in rec['refs']:self.byref.setdefault(r,[]).append(rec)
  self.addrs=[x['rva'] for x in self.ins]
  self.groups={};self.parents={}
  for ent in self.pe.functions:
   root=ent;seen=set()
   while root[2] not in seen:
    seen.add(root[2]);u=self.pe.read(root[2],4)
    if not (u[0]>>3)&4:break
    offset=4+((u[2]+1)&~1)*2
    root=struct.unpack('<III',self.pe.read(root[2]+offset,12))
   self.parents[ent[0]]=root[0];self.groups.setdefault(root[0],[]).append(ent)
  self.group_ends={r:max(x[1] for x in es) for r,es in self.groups.items()}
 def frange(self,addr):
  a=self.pe.containing(addr)
  if not a:return (addr,addr+256,0)
  r=self.parents[a[0]]
  return (r,self.group_ends[r],a[2])
 def fmt(self,i):
  tags=[]
  for r in i['refs']:
   if r in self.names:tags.append(self.names[r])
   if r in self.iat:tags.append(self.iat[r])
   if r in self.strings:tags.append(repr(self.strings[r]))
  return f"{i['rva']:08X}: {i['mnemonic']:9s} {i['operands']}"+(' ; '+' | '.join(tags) if tags else '')
 def dump(self,addr,end=None):
  a,b,_=self.frange(addr) if end is None else (addr,end,0)
  head=f"; {self.pe.path.name} SHA256={__import__('hashlib').sha256(self.pe.data).hexdigest()}\n; ImageBase=0x{self.pe.base:X}, RVAs in left column\n; range 0x{a:X}..0x{b:X}; {self.names.get(a,'unnamed')}\n"
  return head+'\n'.join(self.fmt(i) for i in self.ins[bisect.bisect_left(self.addrs,a):bisect.bisect_left(self.addrs,b)])+'\n'
 def xrefs(self,addr):return [(hex(i['rva']),hex(self.frange(i['rva'])[0]),self.fmt(i)) for i in self.byref.get(addr,[])]
 def overview(self,addr):
  a,b,_=self.frange(addr)
  print(f'Function {a:X}-{b:X} {self.names.get(a,"")}')
  for i in self.ins[bisect.bisect_left(self.addrs,a):bisect.bisect_left(self.addrs,b)]:
   if i['mnemonic'] in ('call','jmp') or any(r in self.strings for r in i['refs']):print(self.fmt(i))
 def vtables(self,pattern):
  out=[]
  for m in re.finditer(rb'\.\?AV[^\x00]{1,160}\x00',self.pe.data):
   name=m.group()[:-1].decode('ascii','replace')
   if not re.search(pattern,name):continue
   td=self.pe.rva(m.start())-16
   for q in re.finditer(re.escape(__import__('struct').pack('<I',td)),self.pe.data):
    co=q.start()-12
    if co<0:continue
    vals=__import__('struct').unpack_from('<6I',self.pe.data,co)
    if vals[0]!=1 or vals[5]!=self.pe.rva(co):continue
    colr=self.pe.rva(co)
    for vp in re.finditer(re.escape(__import__('struct').pack('<Q',self.pe.base+colr)),self.pe.data):
     vt=self.pe.rva(vp.start())+8;entries=[]
     for j in range(80):
      try:p=self.pe.ptr(vt+j*8)-self.pe.base
      except ValueError:break
      sec=self.pe.section(p)
      if not sec or not sec['flags']&0x20000000:break
      entries.append({'offset':j*8,'target_rva':p,'name':self.names.get(p)})
     out.append({'class':name,'vtable_rva':vt,'col_rva':colr,'object_offset':vals[1],'entries':entries})
  return out
if __name__=='__main__':
 import argparse
 ap=argparse.ArgumentParser(description=__doc__)
 ap.add_argument('--dll',required=True)
 ap.add_argument('--asm',required=True)
 ap.add_argument('--strings')
 ap.add_argument('--symbols')
 ap.add_argument('operation',choices=['dump','xrefs','overview','strings','vtables'])
 ap.add_argument('argument')
 ap.add_argument('end',nargs='?')
 a=ap.parse_args();d=Disasm(PE(a.dll),a.asm,a.strings,a.symbols)
 if a.operation=='dump':sys.stdout.write(d.dump(int(a.argument,0),int(a.end,0) if a.end else None))
 elif a.operation=='xrefs':print(json.dumps(d.xrefs(int(a.argument,0)),indent=2))
 elif a.operation=='overview':d.overview(int(a.argument,0))
 elif a.operation=='vtables':print(json.dumps(d.vtables(a.argument),indent=2))
 else:
  for addr,text in d.strings.items():
   if re.search(a.argument,text):
    refs=d.xrefs(addr)
    if refs:print(hex(addr),repr(text),json.dumps(refs))
