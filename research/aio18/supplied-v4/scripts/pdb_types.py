#!/usr/bin/env python3
"""Read plain structure field offsets from the matched PDB's TPI stream."""
import json,pathlib,re,struct,sys
from pdb_index import PDB
class Types:
 def __init__(self,pdb):
  b=pdb.streams[2];ver,h,first,last,n=struct.unpack_from('<5I',b);self.rec={};pos=h
  for i in range(first,last):
   sz,kind=struct.unpack_from('<HH',b,pos);self.rec[i]=(kind,b[pos+4:pos+2+sz]);pos+=sz+2
  assert pos==h+n
 def num(self,b,o):
  v=struct.unpack_from('<H',b,o)[0]
  if v<0x8000:return v,o+2
  formats={0x8000:'b',0x8001:'h',0x8002:'H',0x8003:'i',0x8004:'I',0x8009:'q',0x800a:'Q'}
  f=formats[v];return struct.unpack_from('<'+f,b,o+2)[0],o+2+struct.calcsize(f)
 def struct_info(self,i):
  k,b=self.rec[i]
  if k not in (0x1504,0x1505):return None
  cnt,prop,fields,derived,vshape=struct.unpack_from('<HHIII',b);size,o=self.num(b,16);name=b[o:].split(b'\0')[0].decode('utf8','replace')
  return {'type_index':i,'name':name,'size':size,'member_count':cnt,'forward':bool(prop&128),'field_list':fields}
 def typename(self,i,depth=0):
  prim={0x3:'void',0x10:'signed char',0x20:'unsigned char',0x30:'bool',0x40:'float',0x41:'double',0x70:'char',0x71:'wchar_t',0x72:'int16',0x73:'uint16',0x74:'int32',0x75:'uint32',0x76:'int64',0x77:'uint64',0x12:'int32',0x22:'uint32',0x13:'int64',0x23:'uint64'}
  if i in prim:return prim[i]
  if i<0x1000 and (i&0xff) in prim:return prim[i&0xff]+'*'
  if i not in self.rec or depth>5:return hex(i)
  k,b=self.rec[i];info=self.struct_info(i)
  if info:return info['name']
  if k==0x1002:return self.typename(struct.unpack_from('<I',b)[0],depth+1)+'*'
  if k==0x1001:return 'qualified '+self.typename(struct.unpack_from('<I',b)[0],depth+1)
  if k==0x1503:
   ty,idx=struct.unpack_from('<II',b);n,_=self.num(b,8);return f'{self.typename(ty,depth+1)}[bytes={n}]'
  if k==0x1507:return b[12:].split(b'\0')[0].decode('utf8','replace')
  return f'0x{i:x}(kind=0x{k:x})'
 def fields(self,i):
  if not i:return []
  k,b=self.rec[i];assert k==0x1203;pos=0;out=[]
  while pos<len(b):
   if b[pos]>=0xf0:pos+=b[pos]&15;continue
   kind=struct.unpack_from('<H',b,pos)[0];pos+=2
   if kind==0x150d:
    attr,ty=struct.unpack_from('<HI',b,pos);off,o=self.num(b,pos+6);end=b.index(b'\0',o);name=b[o:end].decode('utf8','replace');pos=end+1
    out.append({'name':name,'offset':off,'type':self.typename(ty),'type_index':ty})
   elif kind==0x1404:
    idx=struct.unpack_from('<I',b,pos+2)[0];out.extend(self.fields(idx));pos+=6
   else:
    out.append({'unparsed_kind':hex(kind),'at':pos-2});break
  return out
 def lookup(self,pattern):
  out=[]
  for i in self.rec:
   s=self.struct_info(i)
   if s and not s['forward'] and re.search(pattern,s['name']):s['fields']=self.fields(s['field_list']);out.append(s)
  return out
if __name__=='__main__':
 t=Types(PDB(sys.argv[1]));print(json.dumps(t.lookup(sys.argv[2]),indent=2))
