import argparse
import bisect
import hashlib
import json
import re
from pathlib import Path

import capstone
import pefile

parser = argparse.ArgumentParser()
parser.add_argument('root', type=Path)
parser.add_argument('output', type=Path)
args = parser.parse_args()
args.output.mkdir(parents=True, exist_ok=True)
focus = re.compile(r'fsr|fidelity|ffx|framegen|swapchain|interop|hudless|uitexture|d3d11on12', re.I)
inventory = []
for file in sorted(args.root.rglob('*.dll')):
    pe = pefile.PE(str(file))
    imports = {}
    for module in getattr(pe, 'DIRECTORY_ENTRY_IMPORT', []):
        imports[module.dll.decode(errors='replace')] = [
            {'name': entry.name.decode(errors='replace') if entry.name else f'ordinal:{entry.ordinal}',
             'iat_rva': hex(entry.address - pe.OPTIONAL_HEADER.ImageBase)} for entry in module.imports]
    exports = [{'name': symbol.name.decode(errors='replace') if symbol.name else f'ordinal:{symbol.ordinal}',
                'rva': hex(symbol.address)} for symbol in getattr(getattr(pe, 'DIRECTORY_ENTRY_EXPORT', None), 'symbols', [])]
    inventory.append({'file': str(file.relative_to(args.root)), 'sha256': hashlib.sha256(file.read_bytes()).hexdigest(),
                      'machine': hex(pe.FILE_HEADER.Machine), 'image_base': hex(pe.OPTIONAL_HEADER.ImageBase),
                      'imports': imports, 'exports': exports,
                      'sections': [{'name': s.Name.rstrip(b'\0').decode(errors='replace'),
                                    'rva': hex(s.VirtualAddress), 'raw_size': s.SizeOfRawData,
                                    'virtual_size': s.Misc_VirtualSize, 'flags': hex(s.Characteristics)} for s in pe.sections]})
    if file.name not in ('SkyrimUpscaler.dll', 'PDPerfPlugin.dll'):
        continue
    prefix = 'plugin' if file.name == 'SkyrimUpscaler.dll' else 'perf'
    blob = file.read_bytes()
    strings = []
    for match in re.finditer(rb'[ -~]{6,1024}', blob):
        value = match.group().decode()
        if focus.search(value):
            try:
                strings.append({'rva': pe.get_rva_from_offset(match.start()), 'value': value})
            except Exception:
                pass
    for match in re.finditer(rb'(?:[ -~]\x00){6,1024}', blob):
        value = match.group().decode('utf-16le')
        if focus.search(value):
            try:
                strings.append({'rva': pe.get_rva_from_offset(match.start()), 'value': value})
            except Exception:
                pass
    strings.sort(key=lambda item: item['rva'])
    targets = [item['rva'] for item in strings]
    functions = sorted([(entry.struct.BeginAddress, entry.struct.EndAddress) for entry in getattr(pe, 'DIRECTORY_ENTRY_EXCEPTION', [])])
    starts = [start for start, _ in functions]
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    decoder.detail = True
    xrefs = []
    for section in pe.sections:
        if not section.Characteristics & 0x20000000:
            continue
        data = section.get_data()
        # Candidate RIP-relative LEA/MOV; decode each candidate before accepting.
        for match in re.finditer(rb'[\x48\x4c][\x8d\x8b][\x05\x0d\x15\x1d\x25\x2d\x35\x3d]', data):
            rva = section.VirtualAddress + match.start()
            insn = next(decoder.disasm(data[match.start():match.start()+15], pe.OPTIONAL_HEADER.ImageBase + rva, count=1), None)
            if insn is None:
                continue
            for operand in insn.operands:
                if operand.type != capstone.CS_OP_MEM or operand.mem.base != capstone.x86.X86_REG_RIP:
                    continue
                target = insn.address + insn.size + operand.mem.disp - pe.OPTIONAL_HEADER.ImageBase
                index = bisect.bisect_right(targets, target) - 1
                if index < 0 or target - targets[index] > 3:
                    continue
                fi = bisect.bisect_right(starts, rva) - 1
                function = functions[fi][0] if fi >= 0 and rva < functions[fi][1] else None
                xrefs.append({'rva': hex(rva), 'instruction': f'{insn.mnemonic} {insn.op_str}',
                              'string_rva': hex(target), 'string': strings[index]['value'],
                              'function_rva': hex(function) if function is not None else None})
    (args.output / f'{prefix}_strings.json').write_text(json.dumps(strings, indent=2), encoding='utf-8')
    (args.output / f'{prefix}_xrefs.json').write_text(json.dumps(xrefs, indent=2), encoding='utf-8')
    interesting = sorted({int(item['function_rva'], 16) for item in xrefs if item['function_rva'] and re.search(r'fsr|ffx|interop|hudless|uitexture', item['string'], re.I)})
    (args.output / f'{prefix}_function_rvas.txt').write_text('\n'.join(hex(rva) for rva in interesting), encoding='utf-8')
    print(json.dumps({'file': file.name, 'sections': inventory[-1]['sections'], 'relevant_strings': len(strings),
                      'decoded_xrefs': len(xrefs), 'target_functions': len(interesting), 'capstone': capstone.__version__}))
(args.output / 'pe_inventory.json').write_text(json.dumps(inventory, indent=2), encoding='utf-8')
print(json.dumps({'dlls': len(inventory), 'output': str(args.output)}))
