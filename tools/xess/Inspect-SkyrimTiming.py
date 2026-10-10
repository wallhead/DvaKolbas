"""Read-only Address Library + PE witnesses for Intel timing integration.

No process attach, executable launch or runtime patch. Relocations identify
candidate functions; decoded instructions and later gameplay traces must
independently establish their meaning and ordering.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile


def addresses(path):
    data = path.read_bytes()
    cursor = 0

    def read(fmt):
        nonlocal cursor
        size = struct.calcsize(fmt)
        value = struct.unpack_from(fmt, data, cursor)
        cursor += size
        return value[0] if len(value) == 1 else value

    fmt = read('<I')
    if fmt not in (1, 2):
        raise ValueError('Only sparse Address Library formats 1/2 are inspected')
    version = read('<4I')
    name_length = read('<I')
    if name_length > 256:
        raise ValueError('Unbounded image name')
    name = data[cursor:cursor + name_length].decode('ascii')
    cursor += name_length
    pointer_size, count = read('<2I')
    if pointer_size != 8 or count > 2_000_000:
        raise ValueError('Unexpected address count/pointer size')

    def decode(kind, previous):
        if kind == 0:
            return read('<Q')
        if kind == 1:
            return previous + 1
        if kind in (2, 3, 4, 5):
            delta = read('<B' if kind < 4 else '<H')
            return previous + delta if kind in (2, 4) else previous - delta
        if kind in (6, 7):
            return read('<H' if kind == 6 else '<I')
        raise ValueError('Invalid compressed address type')

    result = {}
    last_id = last_offset = 0
    for _ in range(count):
        kind = read('<B')
        identity = decode(kind & 15, last_id)
        scaled = bool(kind >> 4 & 8)
        offset = decode(kind >> 4 & 7, last_offset // pointer_size if scaled else last_offset)
        if scaled:
            offset *= pointer_size
        if identity in result:
            raise ValueError('Duplicate relocation ID')
        result[identity] = offset
        last_id, last_offset = identity, offset
    return result, {'sha256': hashlib.sha256(data).hexdigest(), 'version': version, 'image': name, 'entries': count}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', type=Path, required=True)
    parser.add_argument('--library', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    mappings, library = addresses(args.library)
    if tuple(library['version']) != (1, 6, 1170, 0):
        raise ValueError('First timing evidence is for identified 1.6.1170 only')
    pe = pefile.PE(str(args.exe))
    if pe.FILE_HEADER.Machine != 0x8664:
        raise ValueError('Expected AMD64 image')
    base = pe.OPTIONAL_HEADER.ImageBase
    text_section = next(s for s in pe.sections if s.Name.rstrip(b'\0') == b'.text')
    if text_section.get_entropy() > 7.7:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        diagnostic = {'exe': {'bytes': args.exe.stat().st_size, 'sha256': hashlib.sha256(args.exe.read_bytes()).hexdigest()},
                      'address_library': library, 'qualification': 'NOT QUALIFIED',
                      'reason': 'On-disk text section is packed/encrypted; decoding it cannot establish runtime instructions',
                      'text_entropy': text_section.get_entropy(),
                      'candidate_relocations_only': {str(i): hex(mappings[i]) for i in (68617, 36559, 77245, 107142)}}
        args.output.write_text(json.dumps(diagnostic, indent=2), encoding='utf-8')
        print(json.dumps(diagnostic))
        return
    ranges = [(entry.struct.BeginAddress, entry.struct.EndAddress) for entry in pe.DIRECTORY_ENTRY_EXCEPTION]
    engine = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)

    def span(rva):
        return next(((a, b) for a, b in ranges if a <= rva < b), None)

    def instructions(a, b):
        return [{'rva': hex(i.address - base), 'bytes': i.bytes.hex(), 'instruction': i.mnemonic + ' ' + i.op_str}
                for i in engine.disasm(pe.get_data(a, b - a), base + a)]

    candidates = {'poll_input': 68617, 'main_render_world_caller': 36559,
                  'renderer_begin': 77245, 'render_world': 107142}
    output = {'exe': {'bytes': args.exe.stat().st_size, 'sha256': hashlib.sha256(args.exe.read_bytes()).hexdigest()},
              'address_library': library, 'boundaries': {}, 'runtime_ordering': 'NOT QUALIFIED; no live trace'}
    for label, identity in candidates.items():
        rva = mappings[identity]
        containing = span(rva)
        output['boundaries'][label] = {'relocation_id': identity, 'rva': hex(rva), 'unwind_range': containing,
                                      'instructions': instructions(*containing) if containing else []}
    poll = mappings[68617]
    callers = []
    for section in pe.sections:
        if not section.Characteristics & 0x20000000:
            continue
        data = section.get_data()
        for offset, byte in enumerate(data[:-4]):
            if byte != 0xe8:
                continue
            rva = section.VirtualAddress + offset
            if rva + 5 + struct.unpack_from('<i', data, offset + 1)[0] != poll:
                continue
            containing = span(rva)
            if containing is None:
                continue
            decoded = instructions(*containing)
            if not any(i['rva'] == hex(rva) and i['instruction'].startswith('call ') for i in decoded):
                continue
            related = [identity for identity, value in mappings.items() if value == containing[0]]
            callers.append({'call_rva': hex(rva), 'unwind_range': containing, 'relocation_ids': related, 'instructions': decoded})
    output['input_callers'] = callers
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2), encoding='utf-8')
    print(json.dumps({'exe': output['exe'], 'input_callers': [(c['call_rva'], c['relocation_ids']) for c in callers], 'output': str(args.output)}))


if __name__ == '__main__':
    main()
