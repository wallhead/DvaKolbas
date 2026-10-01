#!/usr/bin/env python3
"""Read matched-PDB class members without stopping at base/method/nested records.
Read-only C13/TPI subset; unknown kinds fail explicitly. Inherited offsets are
reported as base records, not flattened or silently guessed.
Format reference: microsoft/microsoft-pdb/include/cvinfo.h (MIT).
"""
from __future__ import annotations
import argparse, json, struct
from pdb_index import PDB
from pdb_types import Types

class ClassTypes(Types):
    def class_fields(self, idx: int, seen: set[int] | None = None) -> list[dict]:
        if not idx: return []
        seen=set() if seen is None else set(seen)
        if idx in seen: raise ValueError('Cyclic LF_INDEX')
        seen.add(idx)
        kind,b=self.rec[idx]
        if kind!=0x1203: raise ValueError(f'Not LF_FIELDLIST: {idx:x}')
        p=0;out=[]
        def name_at(o):
            e=b.index(b'\0',o)
            return b[o:e].decode('utf-8','replace'),e+1
        while p<len(b):
            if b[p]>=0xf0:
                n=b[p]&15
                if not n or p+n>len(b): raise ValueError('Invalid padding')
                p+=n;continue
            start=p;k=struct.unpack_from('<H',b,p)[0];p+=2
            r={'leaf':f'0x{k:04x}','record_offset':start}
            if k in (0x150d,0x1400,0x151a): # MEMBER, BCLASS, BINTERFACE
                attr,ty=struct.unpack_from('<HI',b,p)
                off,p=self.num(b,p+6)
                r.update(kind='member' if k==0x150d else 'base',offset=off,
                         type_index=ty,type=self.typename(ty),attributes=attr)
                if k==0x150d:r['name'],p=name_at(p)
            elif k in (0x1511,0x150f,0x1510,0x1512,0x150e):
                attr,ty=struct.unpack_from('<HI',b,p);p+=6
                r.update(type_index=ty,attributes=attr)
                r['kind']={0x1511:'method',0x150f:'method_group',0x1510:'nested',0x1512:'nested_ex',0x150e:'static_member'}[k]
                if k==0x1511 and ((attr>>2)&7) in (4,6):
                    r['vtable_offset']=struct.unpack_from('<I',b,p)[0];p+=4
                r['name'],p=name_at(p)
            elif k==0x1404: # INDEX has 2 bytes internal padding
                ty=struct.unpack_from('<I',b,p+2)[0];p+=6
                out.extend(self.class_fields(ty,seen));continue
            elif k in (0x1409,0x140a): # VFUNCTAB / FRIENDCLS
                ty=struct.unpack_from('<I',b,p+2)[0];p+=6
                r.update(kind='vfunctab' if k==0x1409 else 'friend_class',type_index=ty)
            elif k==0x1502: # ENUMERATE
                attr=struct.unpack_from('<H',b,p)[0]
                value,p=self.num(b,p+2);r.update(kind='enumerator',value=value,attributes=attr)
                r['name'],p=name_at(p)
            else:
                raise ValueError(f'Unsupported field kind 0x{k:x} at TPI 0x{idx:x}+0x{start:x}')
            out.append(r)
        return out
    def layout(self, idx:int) -> dict:
        s=self.struct_info(idx)
        if not s or s['forward']: raise ValueError('Need complete class/struct')
        records=self.class_fields(s['field_list'])
        s['records']=records
        s['fields']=[x for x in records if x['kind']=='member']
        return s

if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('pdb');ap.add_argument('types',nargs='+',type=lambda x:int(x,0))
    args=ap.parse_args();t=ClassTypes(PDB(args.pdb))
    print(json.dumps([t.layout(i) for i in args.types],indent=2))
