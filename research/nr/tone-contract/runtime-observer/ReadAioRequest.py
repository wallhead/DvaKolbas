"""Read a pinned AIO19 retained request. No debugger, suspension or target writes.

This observes a retained host request, NOT the post-callback vendor frame.
Matching consecutive reads are a consistency check, not an atomic snapshot.
"""
import argparse
import ctypes as c
import ctypes.wintypes as w
import hashlib
import json
import math
from pathlib import Path
import struct
import time
from datetime import datetime, timezone
import pefile
from NativeTextureMetadata import describe_texture, D3D11_SHA

PD_SHA = 'ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435'
NR_SHA = '8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206'


def decode(packet):
    if len(packet) != 0x598 or struct.unpack_from('<II', packet) != (0x598, 1):
        raise ValueError('Retained request header is not the pinned v1 packet')
    count, selected = struct.unpack_from('<II', packet, 0x174)
    if not 1 <= count <= 10:
        raise ValueError('Invalid retained pass count')
    passes = []
    for index in range(count):
        offset = 0x17c + index * 0x68
        identifier, enabled, intensity, structure, tone, skin, style, auto_skin, groups, field24 = struct.unpack_from('<IIffffIIII', packet, offset)
        if enabled not in (0, 1) or auto_skin not in (0, 1) or groups not in (0, 1):
            raise ValueError('Invalid retained boolean control')
        if style > 7 or not all(math.isfinite(v) for v in (intensity, structure, tone, skin)):
            raise ValueError('Invalid retained style/scalar control')
        passes.append(dict(index=index, id=identifier, enabled=enabled, intensity=intensity,
                           structure=structure, tone=tone, skinStructure=skin, style=style,
                           autoSkin=auto_skin, maskGroups=groups, groupCountField=field24))
    resolve = struct.unpack_from('<I', packet, 0x11c)[0]
    scale = struct.unpack_from('<f', packet, 0x120)[0]
    if resolve > 2 or not math.isfinite(scale):
        raise ValueError('Unqualified retained reconstruction controls')
    canonical = 1.0 if scale <= 0 or scale >= 1 else max(scale, 0.25)
    method = 2 if resolve == 2 else int(canonical < 1.0)
    pointers = {key: hex(struct.unpack_from('<Q', packet, offset)[0])
                for key, offset in [('color', 0x10), ('motion', 0x18), ('depth', 0x20), ('output', 0x28)]}
    routing = dict(requestedResolveMethod=resolve, requestedInputScale=scale,
                   canonicalInputScale=canonical, preparationMethodCandidate=method,
                   alternateColorRequested=bool(packet[0x10b]),
                   colorOutputAlias=pointers['color'] == pointers['output'],
                   resourcePointers=pointers,
                   motionScaleRequested=list(struct.unpack_from('<ff', packet, 0xe8)),
                   scope='Retained request plus static selector; not observed shader execution or final vendor frame')
    return dict(passCount=count, selectedPassField=selected, passes=passes, routing=routing)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--pid', type=int, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--samples', type=int, default=15)
    parser.add_argument('--interval-ms', type=int, default=1000)
    args = parser.parse_args()
    if not 1 <= args.samples <= 30 or not 100 <= args.interval_ms <= 1000:
        parser.error('Use 1..30 samples and 100..1000 ms intervals')
    k = c.WinDLL('kernel32', use_last_error=True)
    ps = c.WinDLL('psapi', use_last_error=True)
    k.OpenProcess.argtypes = [w.DWORD, w.BOOL, w.DWORD]; k.OpenProcess.restype = w.HANDLE
    k.CloseHandle.argtypes = [w.HANDLE]
    k.ReadProcessMemory.argtypes = [w.HANDLE, c.c_void_p, c.c_void_p, c.c_size_t, c.POINTER(c.c_size_t)]
    k.ReadProcessMemory.restype = w.BOOL
    k.QueryFullProcessImageNameW.argtypes = [w.HANDLE, w.DWORD, w.LPWSTR, c.POINTER(w.DWORD)]
    k.QueryFullProcessImageNameW.restype = w.BOOL
    k.QueryDosDeviceW.argtypes = [w.LPCWSTR, w.LPWSTR, w.DWORD]; k.QueryDosDeviceW.restype = w.DWORD
    ps.EnumProcessModulesEx.argtypes = [w.HANDLE, c.POINTER(w.HMODULE), w.DWORD, c.POINTER(w.DWORD), w.DWORD]
    ps.EnumProcessModulesEx.restype = w.BOOL
    ps.GetModuleFileNameExW.argtypes = [w.HANDLE, w.HMODULE, w.LPWSTR, w.DWORD]
    ps.GetModuleFileNameExW.restype = w.DWORD
    ps.GetMappedFileNameW.argtypes = [w.HANDLE, c.c_void_p, w.LPWSTR, w.DWORD]
    ps.GetMappedFileNameW.restype = w.DWORD
    process = k.OpenProcess(0x410, False, args.pid)  # QUERY_INFORMATION | VM_READ only
    if not process:
        raise OSError(c.get_last_error(), 'OpenProcess for read-only request capture')
    result = dict(pid=args.pid, startedUtc=datetime.now(timezone.utc).isoformat(), samples=[],
                  scope='Retained AIO host request only; not an atomic or post-callback dispatch snapshot',
                  toolSha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest())

    def read(address, size):
        data = c.create_string_buffer(size); got = c.c_size_t()
        if not k.ReadProcessMemory(process, address, data, size, c.byref(got)) or got.value != size:
            raise OSError(c.get_last_error(), f'ReadProcessMemory {address:#x}')
        return data.raw

    def mapped_path(base):
        name = c.create_unicode_buffer(32768)
        if not ps.GetMappedFileNameW(process, base, name, len(name)):
            raise OSError(c.get_last_error(), 'Mapped file identity')
        for drive in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ':
            device = c.create_unicode_buffer(4096)
            if k.QueryDosDeviceW(drive + ':', device, len(device)):
                prefix = device.value + '\\'
                if name.value.startswith(prefix):
                    return Path(drive + ':\\' + name.value[len(prefix):])
        raise ValueError('Mapped file is not on a resolved local drive')

    try:
        name = c.create_unicode_buffer(32768); length = w.DWORD(len(name))
        if not k.QueryFullProcessImageNameW(process, 0, name, c.byref(length)) or Path(name.value).name.lower() != 'skyrimse.exe':
            raise ValueError('Expected SkyrimSE.exe')
        modules = (w.HMODULE * 4096)(); needed = w.DWORD()
        if not ps.EnumProcessModulesEx(process, modules, c.sizeof(modules), c.byref(needed), 2) or needed.value > c.sizeof(modules):
            raise ValueError('Cannot enumerate all target modules')
        matched = {}; native_modules = []
        for base in modules[:needed.value // c.sizeof(w.HMODULE)]:
            if not ps.GetModuleFileNameExW(process, base, name, len(name)):
                raise OSError(c.get_last_error(), 'Module filename')
            filename = Path(name.value).name.lower()
            if filename == 'd3d11.dll':
                path = mapped_path(base)
                native_modules.append((int(base), path, hashlib.sha256(path.read_bytes()).hexdigest()))
                continue
            if filename not in ('pdperfplugin.dll', 'nvngx_dlssnr.dll'):
                continue
            if filename in matched:
                raise ValueError('Duplicate selected module')
            path = mapped_path(base); data = path.read_bytes(); sha = hashlib.sha256(data).hexdigest()
            expected = PD_SHA if filename == 'pdperfplugin.dll' else NR_SHA
            if sha != expected:
                raise ValueError('Unqualified mapped image: ' + filename)
            matched[filename] = (int(base), path, data)
        if len(matched) != 2:
            raise ValueError('Enable AIO NR and load the save first')
        pd_base, pd_path, pd_data = matched['pdperfplugin.dll']
        nr_base, nr_path, _ = matched['nvngx_dlssnr.dll']
        pe = pefile.PE(data=pd_data)
        # Getter, chain export, backend packet-copy call and pass scalar/style loads.
        witnesses = [(0x9f700, 8), (0x1178d0, 0x3b), (0x110e60, 0x285),
                     (0xa5fd0, 0x30f), (0xa37f1, 0x5f), (0xa0a70, 0x22),
                     (0xa0aa0, 0x20), (0x12fc098, 4), (0x12fc0e0, 4)]
        for rva, size in witnesses:
            if read(pd_base + rva, size) != pe.get_data(rva, size):
                raise ValueError(f'Mapped PD observation code differs at {rva:#x}')
        result.update(pdPath=str(pd_path), pdSha256=PD_SHA, nrPath=str(nr_path), nrSha256=NR_SHA,
                      witnessRanges=[[hex(r), n] for r, n in witnesses],
                      nativeMetadataReaderSha256=hashlib.sha256(Path(__file__).with_name('NativeTextureMetadata.py').read_bytes()).hexdigest())
        native_base = native_modules[0][0] if len(native_modules) == 1 and native_modules[0][2] == D3D11_SHA else None
        result['nativeTextureMetadata'] = dict(qualifiedD3d11Image=bool(native_base),
                                               scope='Retained native texture dimensions/format only; no views, transfer, pixels or GPU ownership proof')
        if native_base:
            result['nativeTextureMetadata'].update(modulePath=str(native_modules[0][1]), moduleSha256=D3D11_SHA)
        for index in range(args.samples):
            backend = struct.unpack('<Q', read(pd_base + 0x136c7c8, 8))[0]
            chain = struct.unpack('<Q', read(pd_base + 0x1375e20, 8))[0]
            if not backend or not chain or struct.unpack('<Q', read(chain + 0x78, 8))[0] != backend:
                raise ValueError('Chain/backend identity is not the pinned route')
            vtable = struct.unpack('<Q', read(chain, 8))[0]
            if not vtable or struct.unpack('<Q', read(vtable + 0x100, 8))[0] != pd_base + 0x110e60:
                raise ValueError('Chain interface method is not the pinned forwarding route')
            first = read(backend + 0x3240, 0x598); second = read(backend + 0x3240, 0x598)
            identity_equal = (struct.unpack('<Q', read(pd_base + 0x136c7c8, 8))[0] == backend
                              and struct.unpack('<Q', read(pd_base + 0x1375e20, 8))[0] == chain
                              and struct.unpack('<Q', read(chain + 0x78, 8))[0] == backend
                              and struct.unpack('<Q', read(chain, 8))[0] == vtable
                              and struct.unpack('<Q', read(vtable + 0x100, 8))[0] == pd_base + 0x110e60)
            sample = dict(index=index, utc=datetime.now(timezone.utc).isoformat(),
                          repeatEqual=first == second, identityEqual=identity_equal)
            if first == second and identity_equal:
                sample.update(decode(first), packetHex=first.hex(),
                              modelCallbackSlot=hex(struct.unpack('<Q', read(nr_base + 0x11527b0, 8))[0]))
                sample['textureMetadata'] = {key: describe_texture(read, int(pointer, 16), native_base)
                                             for key, pointer in sample['routing']['resourcePointers'].items()}
                same_texture_set = (read(backend + 0x3250, 32) == first[0x10:0x30]
                                    and struct.unpack('<Q', read(pd_base + 0x136c7c8, 8))[0] == backend
                                    and struct.unpack('<Q', read(pd_base + 0x1375e20, 8))[0] == chain
                                    and struct.unpack('<Q', read(chain + 0x78, 8))[0] == backend)
                sample['texturePointerSetRepeated'] = same_texture_set
                if not same_texture_set:
                    sample['textureMetadata'] = {key: dict(status='inconsistent', reason='Retained texture set changed during metadata reads')
                                                 for key in sample['routing']['resourcePointers']}
            result['samples'].append(sample)
            if index + 1 < args.samples:
                time.sleep(args.interval_ms / 1000)
        result['acceptedSamples'] = sum('passes' in sample for sample in result['samples'])
        result['rejectedSamples'] = len(result['samples']) - result['acceptedSamples']
        if not result['acceptedSamples']:
            raise ValueError('No matching repeated request reads; no decoded capture')
        result['status'] = 'retained-request-captured'
    except Exception as error:
        result['status'] = 'inconclusive'
        result['error'] = repr(error)
        raise
    finally:
        k.CloseHandle(process)
        result['finishedUtc'] = datetime.now(timezone.utc).isoformat()
        args.output.write_text(json.dumps(result, indent=2, allow_nan=False) + '\n')


if __name__ == '__main__':
    main()
