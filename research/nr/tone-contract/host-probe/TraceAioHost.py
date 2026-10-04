from pathlib import Path
import argparse, hashlib, json
import capstone, pefile

parser=argparse.ArgumentParser(description='Read-only selected unwind ranges from the exact AIO19 host')
parser.add_argument('--host',type=Path,required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('ranges',nargs='*',help='Hex RVA or start:endExclusive')
args=parser.parse_args()
root = args.output
root.mkdir(exist_ok=True)
path = args.host
data = path.read_bytes()
assert hashlib.sha256(data).hexdigest() == 'ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a'
pe = pefile.PE(data=data)
base = pe.OPTIONAL_HEADER.ImageBase
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
md.detail = True
entries = [e.struct for e in pe.DIRECTORY_ENTRY_EXCEPTION]
imports = {i.address-base: i.name.decode() if i.name else str(i.ordinal) for m in pe.DIRECTORY_ENTRY_IMPORT for i in m.imports}
specs = [tuple(int(n,16) for n in x.split(':')) for x in args.ranges] or [(0x2E967B,),(0x2EC4D9,),(0x2FA520,0x2FAC2F),(0x2FD470,)]
targets = [s[0] for s in specs]
records = []
def annotate(i):
    comments=[]
    for op in i.operands:
        if op.type == capstone.CS_OP_MEM and op.mem.base == capstone.x86.X86_REG_RIP:
            dest=i.address+i.size+op.mem.disp-base
            if dest in imports: comments.append('import '+imports[dest])
            try:
                value=pe.get_data(dest,256).split(b'\0',1)[0]
                if len(value)>=4 and all(32<=x<127 for x in value): comments.append(repr(value.decode()))
            except pefile.PEFormatError: pass
    return '; '.join(comments)
for spec in specs:
    target=spec[0]
    entry=next(e for e in entries if e.BeginAddress<=target<e.EndAddress)
    end=spec[1] if len(spec)>1 else entry.EndAddress
    fragments=sorted([f for f in entries if f.BeginAddress<end and f.EndAddress>entry.BeginAddress],key=lambda f:f.BeginAddress)
    ins=[]; covered=entry.BeginAddress
    for f in fragments:
        assert f.BeginAddress<=covered, 'Uncovered bytes in selected range'
        end_fragment=min(end,f.EndAddress)
        raw_fragment=pe.get_data(f.BeginAddress,end_fragment-f.BeginAddress)
        decoded=list(md.disasm(raw_fragment,base+f.BeginAddress))
        assert sum(i.size for i in decoded)==len(raw_fragment), 'Incomplete selected unwind fragment'
        ins.extend(decoded); covered=max(covered,end_fragment)
    assert covered==end
    raw=pe.get_data(entry.BeginAddress,end-entry.BeginAddress)
    rec={'requestedRva':hex(target),'beginRva':hex(entry.BeginAddress),'endRva':hex(end),'unwindFragments':[[hex(f.BeginAddress),hex(min(end,f.EndAddress))] for f in fragments],'sha256':hashlib.sha256(raw).hexdigest(),'instructions':[{'rva':hex(i.address-base),'bytes':i.bytes.hex(),'text':i.mnemonic+' '+i.op_str,'annotation':annotate(i)} for i in ins]}
    (root/f'host-{entry.BeginAddress:x}.asm').write_text('\n'.join(f"{x['rva']} {x['bytes']:28} {x['text']} ; {x['annotation']}" for x in rec['instructions'])+'\n')
    records.append(rec)
    print(hex(target),hex(entry.BeginAddress),hex(end),len(ins))
literals = {}
for needle in [b'EvaluateDLSSNRChain',b'Native frame read-only capture',b'capture implementations',b'CaptureNative',b'InitDLSSNR']:
    positions=[]; offset=0
    while (pos:=data.find(needle,offset))>=0:
        positions.append(pe.get_rva_from_offset(pos)); offset=pos+1
    literals[needle.decode()]=positions
references=[]
interesting=set(x for xs in literals.values() for x in xs)|set(targets)|set(x['beginRva'] and int(x['beginRva'],16) for x in records)
for entry in entries:
    for i in md.disasm(pe.get_data(entry.BeginAddress,entry.EndAddress-entry.BeginAddress),base+entry.BeginAddress):
        for op in i.operands:
            dest=None
            if op.type==capstone.CS_OP_MEM and op.mem.base==capstone.x86.X86_REG_RIP: dest=i.address+i.size+op.mem.disp-base
            elif op.type==capstone.CS_OP_IMM: dest=op.imm-base
            if dest in interesting: references.append({'rva':hex(i.address-base),'functionRva':hex(entry.BeginAddress),'target':hex(dest),'text':i.mnemonic+' '+i.op_str})
report={'binarySha256':hashlib.sha256(data).hexdigest(),'scope':'Selected complete PE unwind fragments; xref scan of unwind-covered code only; no live host execution claim','functions':records,'literals':{k:[hex(x) for x in v] for k,v in literals.items()},'references':references,'toolSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'latest.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({'literals':report['literals'],'references':references},indent=2))
