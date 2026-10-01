#!/usr/bin/env python3
"""Read MSF7 PDB identity and address-bearing CodeView symbol records."""
import json,pathlib,struct,sys,uuid
sys.path.insert(0,str(pathlib.Path(__file__).parent));from pe_index import PE
class PDB:
 def __init__(self,path):
  self.data=pathlib.Path(path).read_bytes();b=self.data
  assert b.startswith(b'Microsoft C/C++ MSF 7.00\r\n\x1aDS\0\0\0')
  self.bs,_,_,ds,_,bm=struct.unpack_from('<6I',b,32)
  blocks=struct.unpack_from('<'+'I'*((ds+self.bs-1)//self.bs),b,bm*self.bs)
  d=b''.join(b[x*self.bs:(x+1)*self.bs] for x in blocks)[:ds]
  ns=struct.unpack_from('<I',d)[0];sizes=struct.unpack_from('<'+'I'*ns,d,4);pos=4+4*ns;self.streams=[]
  for sz in sizes:
   if sz==0xffffffff:self.streams.append(None);continue
   n=(sz+self.bs-1)//self.bs;pages=struct.unpack_from('<'+'I'*n,d,pos);pos+=4*n
   self.streams.append(b''.join(b[x*self.bs:(x+1)*self.bs] for x in pages)[:sz])
  info=self.streams[1];self.identity={'guid':str(uuid.UUID(bytes_le=info[12:28])),'age':struct.unpack_from('<I',info,8)[0]}
 def records(self,dat,source,skip=0):
  pos=skip
  while pos+4<=len(dat):
   size,typ=struct.unpack_from('<HH',dat,pos)
   if size<2 or pos+size+2>len(dat):break
   body=dat[pos+4:pos+size+2];rec=None
   if typ==0x110e and len(body)>=11:
    flags,off,seg=struct.unpack_from('<IIH',body);rec={'kind':'S_PUB32','offset':off,'section':seg,'name':body[10:].split(b'\0')[0].decode('utf8','replace'),'flags':flags}
   elif typ in (0x110f,0x1110,0x1146,0x1147) and len(body)>=36:
    _,_,_,length,_,_,_,off,seg,flags=struct.unpack_from('<8IHB',body)
    rec={'kind':hex(typ),'offset':off,'section':seg,'name':body[35:].split(b'\0')[0].decode('utf8','replace'),'size':length,'flags':flags}
   elif typ in (0x110c,0x110d) and len(body)>=11:
    _,off,seg=struct.unpack_from('<IIH',body);rec={'kind':hex(typ),'offset':off,'section':seg,'name':body[10:].split(b'\0')[0].decode('utf8','replace')}
   if rec:rec['source']=source;rec['record_offset']=pos;yield rec
   pos+=size+2
 def symbols(self):
  dbi=self.streams[3];sr=struct.unpack_from('<H',dbi,20)[0]
  if sr!=65535:yield from self.records(self.streams[sr],'global:'+str(sr))
  modsz=struct.unpack_from('<I',dbi,24)[0];end=64+modsz;pos=64
  while pos+64<=end:
   stream=struct.unpack_from('<H',dbi,pos+34)[0];symbytes=struct.unpack_from('<I',dbi,pos+36)[0]
   first=dbi.find(b'\0',pos+64,end);second=dbi.find(b'\0',first+1,end)
   if first<0 or second<0:break
   name=dbi[pos+64:first].decode('utf8','replace')
   if stream!=65535 and stream<len(self.streams) and symbytes>4:
    yield from self.records(self.streams[stream][:symbytes],name,4)
   pos=(second+1+3)&~3
if __name__=='__main__':
 pp=pathlib.Path(sys.argv[1]);pe=PE(sys.argv[2]);out=pathlib.Path(sys.argv[3]);p=PDB(pp)
 syms=[]
 for s in p.symbols():
  si=s['section']
  if 0<si<=len(pe.sections):s['rva']=pe.sections[si-1]['rva']+s['offset']
  syms.append(s)
 ident={'pdb':pp.name,**p.identity,'dll_codeview':pe.codeview(),'matches':any(x['guid']==p.identity['guid'] and x['age']==p.identity['age'] for x in pe.codeview()),'symbol_records':len(syms)}
 out.with_suffix('.identity.json').write_text(json.dumps(ident,indent=2));out.with_suffix('.symbols.json').write_text(json.dumps(syms,indent=2));print(ident)
 with out.with_suffix('.symbols.tsv').open('w') as f:
  f.write('RVA\tSize\tKind\tName\n')
  for s in syms:f.write(f"0x{s.get('rva',0):08X}\t{s.get('size','')}\t{s['kind']}\t{s['name']}\n")
