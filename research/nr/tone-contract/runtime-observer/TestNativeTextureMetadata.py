"""Owned D3D11 GetDesc reference versus passive memory reads. No game attachment."""
import argparse
import ctypes as c
import ctypes.wintypes as w
import hashlib
import json
from pathlib import Path
import subprocess
from NativeTextureMetadata import describe_texture, D3D11_SHA, GETTER_RVA


class Sample(c.Structure):
    _fields_ = [('Count', w.UINT), ('Quality', w.UINT)]


class Desc(c.Structure):
    _fields_ = [('Width', w.UINT), ('Height', w.UINT), ('MipLevels', w.UINT),
               ('ArraySize', w.UINT), ('Format', w.UINT), ('SampleDesc', Sample),
               ('Usage', w.UINT), ('BindFlags', w.UINT), ('CPUAccessFlags', w.UINT), ('MiscFlags', w.UINT)]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if b'SkyrimSE.exe' in subprocess.check_output(['tasklist', '/FI', 'IMAGENAME eq SkyrimSE.exe', '/FO', 'CSV', '/NH']):
        raise SystemExit('Close Skyrim before owned GPU allocation')
    assert c.sizeof(Desc) == 44
    k = c.WinDLL('kernel32', use_last_error=True); d = c.WinDLL('d3d11', use_last_error=True)
    d.D3D11CreateDevice.argtypes = [c.c_void_p, w.UINT, w.HMODULE, w.UINT, c.c_void_p, w.UINT, w.UINT,
                                  c.POINTER(c.c_void_p), c.POINTER(w.UINT), c.POINTER(c.c_void_p)]
    d.D3D11CreateDevice.restype = w.LONG
    k.GetModuleHandleExW.argtypes = [w.DWORD, c.c_void_p, c.POINTER(w.HMODULE)]; k.GetModuleHandleExW.restype = w.BOOL
    k.GetModuleFileNameW.argtypes = [w.HMODULE, w.LPWSTR, w.DWORD]; k.GetModuleFileNameW.restype = w.DWORD
    k.ReadProcessMemory.argtypes = [w.HANDLE, c.c_void_p, c.c_void_p, c.c_size_t, c.POINTER(c.c_size_t)]
    k.ReadProcessMemory.restype = w.BOOL

    def method(obj, slot):
        return c.cast(c.c_void_p.from_address(obj).value, c.POINTER(c.c_void_p))[slot]

    def release(obj):
        c.WINFUNCTYPE(w.ULONG, c.c_void_p)(method(obj, 2))(obj)

    def read(address, size):
        data = c.create_string_buffer(size); got = c.c_size_t()
        if not k.ReadProcessMemory(w.HANDLE(-1), address, data, size, c.byref(got)) or got.value != size:
            raise OSError(c.get_last_error(), 'Owned fixture memory read')
        return data.raw

    device = c.c_void_p(); context = c.c_void_p(); level = w.UINT()
    assert d.D3D11CreateDevice(None, 1, None, 0, None, 0, 7, c.byref(device), c.byref(level), c.byref(context)) >= 0
    textures = []; rows = []; negative = []
    try:
        create = c.WINFUNCTYPE(w.LONG, c.c_void_p, c.POINTER(Desc), c.c_void_p, c.POINTER(c.c_void_p))(method(device.value, 5))
        cases = [(f, 1, 1, 1, 0, 8, 0) for f in [28, 10, 41, 34, 27, 19, 44, 39]]
        cases += [(28, 2, 3, 1, 0, 8, 0), (28, 1, 1, 4, 0, 8, 0), (28, 1, 1, 1, 3, 0, 0x20000)]
        for index, (fmt, mips, array, samples, usage, bind, cpu) in enumerate(cases):
            requested = Desc(32 + index * 16, 24 + index * 8, mips, array, fmt, Sample(samples, 0), usage, bind, cpu, 0)
            texture = c.c_void_p()
            assert create(device, c.byref(requested), None, c.byref(texture)) >= 0
            textures.append(texture.value)
            getter = method(texture.value, 10); actual = Desc()
            # This COM call is only in our owned fixture, never in the passive reader.
            c.WINFUNCTYPE(None, c.c_void_p, c.POINTER(Desc))(getter)(texture, c.byref(actual))
            assert bytes(actual) == bytes(requested)
            module = w.HMODULE(); assert k.GetModuleHandleExW(6, getter, c.byref(module))
            path = c.create_unicode_buffer(32768); assert k.GetModuleFileNameW(module, path, len(path))
            assert hashlib.sha256(Path(path.value).read_bytes()).hexdigest() == D3D11_SHA
            assert getter == module.value + GETTER_RVA
            expected = dict(width=actual.Width, height=actual.Height, mipLevels=actual.MipLevels,
                            arraySize=actual.ArraySize, format=actual.Format,
                            sampleCount=actual.SampleDesc.Count, sampleQuality=actual.SampleDesc.Quality)
            passive = describe_texture(read, texture.value, module.value)
            assert passive['status'] == 'captured', passive
            assert all(passive[key] == value for key, value in expected.items()), (expected, passive)
            rows.append(dict(case=index, expected=expected, passive=passive))
            if index == 0:
                vtable = c.c_void_p.from_address(texture.value).value
                def wrong_method(address, size):
                    if address == vtable + 80 and size == 8:
                        return (getter + 1).to_bytes(8, 'little')
                    return read(address, size)
                def changed_code(address, size):
                    value = read(address, size)
                    return bytes([value[0] ^ 1]) + value[1:] if address == getter else value
                calls = 0
                def changed_identity(address, size):
                    nonlocal calls
                    if address == texture.value and size == 8:
                        calls += 1
                        if calls > 1:
                            return (vtable + 8).to_bytes(8, 'little')
                    return read(address, size)
                def changed_field(address, size):
                    value = read(address, size)
                    if address == texture.value + 0x148 and size == 8:
                        nonlocal_calls[0] += 1
                        if nonlocal_calls[0] > 1:
                            return (actual.Width + 1).to_bytes(4, 'little') + value[4:]
                    return value
                nonlocal_calls = [0]
                for label, reader, base in [('unknown-module', read, None), ('unknown-getter', wrong_method, module.value),
                                            ('changed-code', changed_code, module.value), ('changed-identity', changed_identity, module.value),
                                            ('changing-fields', changed_field, module.value)]:
                    rejected = describe_texture(reader, texture.value, base)
                    assert rejected['status'] != 'captured', (label, rejected)
                    negative.append(label)
    finally:
        for texture in textures:
            release(texture)
        release(context.value); release(device.value)
    report = dict(scope='Owned hardware textures only; no target COM calls or image capture', cases=rows,
                  refusedCases=negative, fixtureSha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                  readerSha256=hashlib.sha256(Path(__file__).with_name('NativeTextureMetadata.py').read_bytes()).hexdigest(),
                  d3d11Sha256=D3D11_SHA)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(f'PASS {len(rows)} owned texture descriptions; {len(negative)} refusal cases')


if __name__ == '__main__':
    main()
