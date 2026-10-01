import argparse
import bisect
import json
from pathlib import Path
import capstone
import pefile

p = argparse.ArgumentParser()
p.add_argument('binary', type=Path)
p.add_argument('output', type=Path)
p.add_argument('rvas', nargs='+', type=lambda s: int(s, 0))
a = p.parse_args()
pe = pefile.PE(str(a.binary))
base = pe.OPTIONAL_HEADER.ImageBase
funcs = sorted((f.struct.BeginAddress, f.struct.EndAddress) for f in pe.DIRECTORY_ENTRY_EXCEPTION)
starts = [f[0] for f in funcs]
iat = {i.address: (m.dll.decode() + '!' + (i.name.decode() if i.name else str(i.ordinal)))
       for m in pe.DIRECTORY_ENTRY_IMPORT for i in m.imports}
exports = {base+e.address: e.name.decode() for e in pe.DIRECTORY_ENTRY_EXPORT.symbols if e.name}
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
md.detail = True
with a.output.open('w', encoding='utf-8') as out:
    for target in a.rvas:
        index = bisect.bisect_right(starts, target)-1
        start,end = funcs[index]
        if not start <= target < end:
            start,end = target,target+0x400
        out.write(f'\n=== RVA {start:#x}..{end:#x} ===\n')
        for i in md.disasm(pe.get_data(start, end-start), base+start):
            annotations = []
            for op in i.operands:
                if op.type == capstone.CS_OP_MEM and op.mem.base == capstone.x86.X86_REG_RIP:
                    address = i.address+i.size+op.mem.disp
                    if address in iat: annotations.append(iat[address])
                    if address in exports: annotations.append(exports[address])
                if op.type == capstone.CS_OP_IMM and op.imm in exports:
                    annotations.append(exports[op.imm])
            out.write(f'{i.address-base:08x}  {i.bytes.hex():<28} {i.mnemonic} {i.op_str}'+
                      (' ; '+' | '.join(annotations) if annotations else '')+'\n')
print(str(a.output))
