import hashlib
import json
import pathlib
import struct
import sys
import zipfile

import capstone
import pefile

root = pathlib.Path(__file__).resolve().parents[1]
archive = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else pathlib.Path(r'C:\Users\user\Downloads\AIO18_FSR_RE_Evidence.zip')
extracted = root / 'aio-build18'
output = root / ('evidence-review-v2' if archive.stem.endswith('_v2') else 'evidence-review')
output.mkdir(exist_ok=True)

def digest(data):
    return hashlib.sha256(data).hexdigest()

with zipfile.ZipFile(archive) as z:
    names = {n.replace('\\', '/'): n for n in z.namelist()}
    checksums = z.read(names['SHA256SUMS.txt']).decode().splitlines()
    for line in checksums:
        expected, name = line.split('  ', 1)
        assert digest(z.read(names[name])) == expected, name
    for name, raw in names.items():
        target = (output / name).resolve()
        assert target.is_relative_to(output.resolve()), name
        assert not target.exists(), f'Refusing to overwrite {target}'
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(z.read(raw))

manifest = json.loads((output / 'evidence/manifest.json').read_text())
original = pathlib.Path(r'C:\Users\user\Downloads\SkyrimUpscalerAIOBuild18-Hotfix1.7z')
assert original.stat().st_size == manifest['archive']['bytes']
assert digest(original.read_bytes()) == manifest['archive']['sha256']
files = {}
for record in manifest['files']:
    target = (extracted / record['path']).resolve()
    assert target.is_relative_to(extracted.resolve())
    data = target.read_bytes()
    assert len(data) == record['bytes'] and digest(data) == record['sha256'], record['path']
    files[target.name] = target

modules = {}
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
landmarks = json.loads((output / 'evidence/landmarks.json').read_text())
if (output / 'evidence/v2/landmarks_v2.json').exists():
    landmarks.extend(json.loads((output / 'evidence/v2/landmarks_v2.json').read_text()))
decoded = []
for record in landmarks:
    name = record['module']
    if name not in modules:
        modules[name] = pefile.PE(str(files[name]))
    pe = modules[name]
    assert digest(files[name].read_bytes()) == record['sha256']
    rva = int(record['rva'], 0)
    expected = bytes.fromhex(record['bytes'])
    assert pe.get_data(rva, len(expected)) == expected, record
    instructions = list(decoder.disasm(expected, rva))
    assert sum(i.size for i in instructions) == len(expected), record
    decoded.append({'module': name, 'rva': record['rva'], 'decoded': [f'{i.mnemonic} {i.op_str}' for i in instructions]})

for name, slot, target in [
    ('PDPerfPlugin.dll', 0x11a82d8 + 8, 0xd0cb0),
    ('PDPerfPlugin.dll', 0x11a9f28 + 8, 0xef230),
    ('amd_fidelityfx_framegeneration_dx12.dll', 0x1073e8 + 0x160, 0x94a0),
    ('amd_fidelityfx_framegeneration_dx12.dll', 0x1073e8 + 0x168, 0x98e0),
]:
    pe = modules[name]
    assert struct.unpack('<Q', pe.get_data(slot, 8))[0] - pe.OPTIONAL_HEADER.ImageBase == target

host = modules['SkyrimUpscaler.dll']
delays = {(d.dll.decode(), i.name.decode()): i.address - host.OPTIONAL_HEADER.ImageBase
          for d in host.DIRECTORY_ENTRY_DELAY_IMPORT for i in d.imports if i.name}
assert delays['PDPerfPlugin.dll', 'EvaluateUpscaler'] == 0x472c20
assert delays['PDPerfPlugin.dll', 'EvaluateFrameGeneration'] == 0x472c58
version_pe = pefile.PE(str(files['ffx_fsr3upscaler_x64.dll']))
version_code = version_pe.get_data(0x42e0, 6)
assert version_code == bytes.fromhex('b80210c000c3')
result = {
    'status': 'PASS', 'scope': 'Independent hash/PE/Capstone byte checks; no runtime execution',
    'evidence_zip_sha256': digest(archive.read_bytes()),
    'zip_members_hashed': len(checksums), 'original_files_hashed': len(manifest['files']),
    'landmarks_hashed_and_decoded': len(landmarks), 'vtable_slots_checked': 4,
    'host_delay_imports_checked': 2, 'legacy_effect_version': '3.1.2',
    'decoded_landmarks': decoded,
}
(output / 'independent-verification.json').write_text(json.dumps(result, indent=2))
print(json.dumps({k: v for k, v in result.items() if k != 'decoded_landmarks'}, indent=2))
