"""Read bounded instruction witnesses from the user's running Skyrim.

OpenProcess requests QUERY_INFORMATION and VM_READ only. This does not inject,
patch, launch, suspend, change settings or dump the complete executable. Static
instruction flow can guide hooks; it does not itself prove per-frame timing.
"""
import argparse
import ctypes as c
from ctypes import wintypes as w
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import runpy
import struct

import capstone
import pefile

EXE_SHA = 'c434208894f07f604b852f29b8edc3a58c4de63de783373733e72b2b73f33be9'
LIB_SHA = 'c4093c569a3c83b26587f4b9ea4c55de9ae6e73b84a2af9fb3fbd30e2fe0d452'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--pid', type=int, required=True)
    parser.add_argument('--library', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--scan-input-callers', action='store_true', help='Scan at most 32 MiB of mapped code; save bounded input/update/render caller witnesses')
    args = parser.parse_args()
    if hashlib.sha256(args.library.read_bytes()).hexdigest() != LIB_SHA:
        raise ValueError('First timing capture requires the identified 1.6.1170 address library')
    loader = runpy.run_path(str(Path(__file__).with_name('Inspect-SkyrimTiming.py')))
    addresses, library = loader['addresses'](args.library)
    kernel = c.WinDLL('kernel32', use_last_error=True)
    psapi = c.WinDLL('psapi', use_last_error=True)
    kernel.OpenProcess.argtypes = [w.DWORD, w.BOOL, w.DWORD]
    kernel.OpenProcess.restype = w.HANDLE
    kernel.CloseHandle.argtypes = [w.HANDLE]
    kernel.QueryFullProcessImageNameW.argtypes = [w.HANDLE, w.DWORD, w.LPWSTR, c.POINTER(w.DWORD)]
    kernel.QueryFullProcessImageNameW.restype = w.BOOL
    kernel.ReadProcessMemory.argtypes = [w.HANDLE, c.c_void_p, c.c_void_p, c.c_size_t, c.POINTER(c.c_size_t)]
    kernel.ReadProcessMemory.restype = w.BOOL
    psapi.EnumProcessModulesEx.argtypes = [w.HANDLE, c.POINTER(w.HMODULE), w.DWORD, c.POINTER(w.DWORD), w.DWORD]
    psapi.EnumProcessModulesEx.restype = w.BOOL
    psapi.GetModuleFileNameExW.argtypes = [w.HANDLE, w.HMODULE, w.LPWSTR, w.DWORD]
    psapi.GetModuleFileNameExW.restype = w.DWORD
    process = kernel.OpenProcess(0x410, False, args.pid)
    if not process:
        raise OSError(c.get_last_error(), 'Read-only Skyrim access')
    try:
        name = c.create_unicode_buffer(32768)
        size = w.DWORD(len(name))
        if not kernel.QueryFullProcessImageNameW(process, 0, name, c.byref(size)):
            raise OSError(c.get_last_error(), 'Process image identity')
        image_path = Path(name.value)
        if image_path.name.lower() != 'skyrimse.exe' or hashlib.sha256(image_path.read_bytes()).hexdigest() != EXE_SHA:
            raise ValueError('Only the identified local Skyrim 1.6.1170 image is admitted')
        modules = (w.HMODULE * 1024)()
        needed = w.DWORD()
        if not psapi.EnumProcessModulesEx(process, modules, c.sizeof(modules), c.byref(needed), 2) or needed.value > c.sizeof(modules):
            raise OSError(c.get_last_error(), 'Bounded module enumeration')
        bases = []
        for module in modules[:needed.value // c.sizeof(w.HMODULE)]:
            if not psapi.GetModuleFileNameExW(process, module, name, len(name)):
                raise OSError(c.get_last_error(), 'Module identity')
            if Path(name.value).resolve() == image_path.resolve():
                bases.append(int(module))
        if len(bases) != 1:
            raise ValueError('Require exactly one mapped Skyrim image')
        base = bases[0]
        pe = pefile.PE(str(image_path))
        ranges = [(entry.struct.BeginAddress, entry.struct.EndAddress) for entry in pe.DIRECTORY_ENTRY_EXCEPTION]
        engine = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)

        def read(rva, count):
            if count <= 0 or count > 65536 or rva + count > pe.OPTIONAL_HEADER.SizeOfImage:
                raise ValueError('Instruction capture exceeds bounded image region')
            data = c.create_string_buffer(count)
            actual = c.c_size_t()
            if not kernel.ReadProcessMemory(process, base + rva, data, count, c.byref(actual)) or actual.value != count:
                raise OSError(c.get_last_error(), f'Read instruction range {rva:#x}')
            return data.raw

        witnesses = {}
        total_bytes = 0
        for label, identity in {'poll_input': 68617, 'main_update': 36564, 'main_render_world_caller': 36559,
                                'renderer_begin': 77245, 'render_world': 107142}.items():
            rva = addresses[identity]
            begin, end = next((a, b) for a, b in ranges if a <= rva < b)
            data = read(begin, end - begin)
            if data != read(begin, end - begin):
                raise ValueError('Instruction range changed during capture; no stable witness')
            instructions = [{'rva': hex(i.address - base), 'bytes': i.bytes.hex(), 'instruction': i.mnemonic + ' ' + i.op_str}
                            for i in engine.disasm(data, base + begin)]
            witnesses[label] = {'relocation_id': identity, 'rva': hex(rva), 'unwind_range': [hex(begin), hex(end)],
                                'sha256': hashlib.sha256(data).hexdigest(), 'bytes_captured': len(data), 'code_hex': data.hex(), 'instructions': instructions}
            total_bytes += len(data)
        input_callers = []
        scanned_bytes = 0
        if args.scan_input_callers:
            section = next(s for s in pe.sections if s.Name.rstrip(b'\0') == b'.text')
            extent = section.Misc_VirtualSize
            if extent > 32 * 1024 * 1024:
                raise ValueError('Mapped code scan exceeds 32 MiB bound')
            # No whole-image dump: examine code in bounded chunks and retain
            # only instruction witnesses for direct callers of the input poll.
            targets = {addresses[68617]: 'poll_input', addresses[36564]: 'main_update',
                       addresses[36559]: 'main_render_world_caller', addresses[77245]: 'renderer_begin'}
            seen = set()
            counts = {name: 0 for name in targets.values()}
            for offset in range(0, extent, 65532):
                start = section.VirtualAddress + offset
                chunk = read(start, min(65536, extent - offset))
                scanned_bytes += len(chunk)
                for index, byte in enumerate(chunk[:-4]):
                    call_rva = start + index
                    if byte != 0xe8 or call_rva in seen:
                        continue
                    target = call_rva + 5 + struct.unpack_from('<i', chunk, index + 1)[0]
                    if target not in targets:
                        continue
                    containing = next(((a, b) for a, b in ranges if a <= call_rva < b), None)
                    if containing is None:
                        continue
                    begin, end = containing
                    code = read(begin, end - begin)
                    if code != read(begin, end - begin):
                        raise ValueError('Input caller instructions changed during capture')
                    instructions = [{'rva': hex(i.address - base), 'bytes': i.bytes.hex(), 'instruction': i.mnemonic + ' ' + i.op_str}
                                    for i in engine.disasm(code, base + begin)]
                    if not any(i['rva'] == hex(call_rva) and i['instruction'].startswith('call ') for i in instructions):
                        continue
                    if counts[targets[target]] == 8 or total_bytes + len(code) > 65536:
                        raise ValueError('Caller count or saved instruction bytes exceed capture bound')
                    seen.add(call_rva)
                    counts[targets[target]] += 1
                    input_callers.append({'target': targets[target], 'call_rva': hex(call_rva), 'unwind_range': [hex(begin), hex(end)],
                                          'relocation_ids': [identity for identity, rva in addresses.items() if rva == begin],
                                          'code_hex': code.hex(), 'instructions': instructions})
                    total_bytes += len(code)
        result = {'pid': args.pid, 'utc': datetime.now(timezone.utc).isoformat(), 'process_image': str(image_path),
                  'exe_sha256': EXE_SHA, 'address_library': library, 'module_base': hex(base),
                  'bytes_captured': total_bytes, 'access': 'QUERY_INFORMATION | VM_READ only',
                  'mapped_code_scanned_bytes': scanned_bytes, 'input_callers': input_callers,
                  'witnesses': witnesses, 'engine_timing_qualification': 'NOT QUALIFIED; instruction capture is not per-frame ordering trace'}
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(result, indent=2), encoding='utf-8')
        print(json.dumps({'output': str(args.output), 'bytes_captured': total_bytes, 'engine_timing_qualification': result['engine_timing_qualification']}))
    finally:
        kernel.CloseHandle(process)


if __name__ == '__main__':
    main()
