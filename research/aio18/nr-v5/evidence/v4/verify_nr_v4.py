from pathlib import Path
import hashlib, json, struct, sys

root = Path(__file__).resolve().parents[2]
host_path = Path('/mnt/data/aio18_nr_work/SkyrimUpscaler.dll')
pd_path = Path('/mnt/data/aio18_nr_work/PDPerfPlugin.dll')
nr_path = Path('/mnt/data/aio18_nr_work/nvngx_dlssnr.dll')
checks = []

def ok(name, value, detail=None):
    item = {'check': name, 'passed': bool(value)}
    if detail is not None:
        item['detail'] = detail
    checks.append(item)

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

expected_hashes = {
    'SkyrimUpscaler.dll': '5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81',
    'PDPerfPlugin.dll': '53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1',
    'nvngx_dlssnr.dll': '8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206',
}
for p in (host_path, pd_path, nr_path):
    ok(f'{p.name} hash', p.exists() and sha(p) == expected_hashes[p.name], sha(p) if p.exists() else 'missing')

host = host_path.read_bytes()
pd = pd_path.read_bytes()

def sections(data):
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    count = struct.unpack_from('<H', data, pe + 6)[0]
    opt_size = struct.unpack_from('<H', data, pe + 20)[0]
    section_table = pe + 24 + opt_size
    result = []
    for i in range(count):
        off = section_table + i * 40
        name = data[off:off+8].split(b'\0')[0].decode('ascii', 'replace')
        virtual_size, virtual_address, raw_size, raw_offset = struct.unpack_from('<IIII', data, off + 8)
        result.append((name, virtual_address, virtual_size, raw_offset, raw_size))
    return result

def rva_to_offset(data, rva):
    for name, va, vs, raw, raw_size in sections(data):
        if va <= rva < va + max(vs, raw_size):
            return raw + (rva - va)
    raise ValueError(f'RVA {rva:#x} not mapped')

def at(data, rva, expected_hex):
    expected = bytes.fromhex(expected_hex)
    off = rva_to_offset(data, rva)
    actual = data[off:off+len(expected)]
    return actual == expected, actual.hex()

landmarks = [
    ('host primary reset zero source', host, 0x2A23BB, '45 33 e4'),
    ('host primary reset store external+100', host, 0x2A24E5, '44 88 65 20'),
    ('host before-SR NR call', host, 0x294E24, 'e8 d7 d4 00 00'),
    ('host following SR import call', host, 0x294FBD, 'ff 15 5d dc 1d 00'),
    ('host late NR call', host, 0x2AB0CF, 'e8 2c 72 ff ff'),
    ('host following FSR-FG producer call', host, 0x2AB349, 'e8 52 8e fe ff'),
    ('host CS bridge r9=1', host, 0x265AF9, '41 b1 01'),
    ('host CS bridge r8=r9', host, 0x265AFC, '45 0f b6 c1'),
    ('host CS common NR call', host, 0x265B0A, 'e8 f1 c7 03 00'),
    ('PD first-pass external reset pointer', pd, 0x9F37A, '4d 8d 8d 00 01 00 00'),
    ('PD extra-pass reset load', pd, 0x9F3A2, '41 0f b6 01'),
    ('PD clear extra-pass reset after success', pd, 0x9F408, 'c6 06 00'),
    ('PD new feature arms reset latch', pd, 0xA1B3B, 'c6 84 18 c8 00 00 00 01'),
    ('PD extra ReleaseFeature call', pd, 0xA18DC, 'e8 7f 28 00 00'),
    ('PD released feature pointer clear', pd, 0xA1931, '48 c7 44 df 48 00 00 00 00'),
    ('PD released feature reset clear', pd, 0xA193A, 'c6 84 3b c8 00 00 00 00'),
    ('PD device-recreate shutdown call', pd, 0xA185B, 'e8 e0 05 00 00'),
    ('PD device-recreate reinit call', pd, 0xA1878, 'e8 23 b1 ff ff'),
    ('PD primary ReleaseFeature call', pd, 0xA1C4F, 'e8 0c 25 00 00'),
    ('PD DestroyParameters call', pd, 0xA2466, 'e8 05 07 06 00'),
    ('PD release-router backend call', pd, 0xF4E5C, 'e8 bf ce fa ff'),
]
for name, data, rva, hexbytes in landmarks:
    passed, actual = at(data, rva, hexbytes)
    ok(name, passed, f'RVA={rva:#x} actual={actual}')

strings = [
    b'DLSSNR: ReinitAfterDeviceRecreate %s',
    b'DLSSNR: BeginCommandList failed (lost=%d device=%p queue=%p list=%p fence=%p), recreating SwapChain resources',
    b'DLSSNR: BeginCommandList still failed while creating feature',
    b'DLSSNR: extra pass %u failed, keeping %u pass(es)',
    b'DLSSNR: skip local eval, depth=%p motion=%p (need committed GPU1 staging, not NT-shared)',
    b'DLSSNR: failed to copy NT-shared backbuffer to local staging',
    b'DLSSNR: skip present eval, committed color/output missing',
]
for s in strings:
    ok('PD diagnostic: ' + s.decode('ascii'), s in pd)

report = (root / 'AIO18_NR_RE_v4.md').read_text('utf-8')
for marker in [
    '4 / 4 static NR passes are complete',
    'common primary Reset = 0',
    'first-use reset latches',
    'two recovery levels',
    'SR → NR → FSR FG preparation',
    'HIGH-CONFIDENCE PARITY MISMATCH',
    'Pass 5 — runtime validation',
]:
    ok('report marker: ' + marker, marker in report)

for filename in [
    'recovered_lifecycle_contract.json',
    'integration_order.json',
    'dvakolbas_lifecycle_comparison.json',
    'nr_lifecycle_diagnostics.txt',
]:
    ok('evidence file exists: ' + filename, (root / 'evidence/v4' / filename).is_file())

# Package policy: evidence must not redistribute analyzed vendor binaries/PDBs.
forbidden = []
for p in root.rglob('*'):
    if p.is_file() and p.suffix.lower() in {'.dll', '.exe', '.pdb', '.lib'}:
        forbidden.append(str(p.relative_to(root)))
ok('no vendor binary/PDB artifacts in evidence tree', not forbidden, forbidden)

out = {
    'scope': 'Fresh static verification against exact original AIO18 module bytes. No DLL/GPU execution.',
    'module_hashes': expected_hashes,
    'checks': checks,
    'passed': sum(x['passed'] for x in checks),
    'failed': sum(not x['passed'] for x in checks),
}
output = root / 'evidence/v4/verification_v4.json'
output.write_text(json.dumps(out, indent=2), encoding='utf-8')
print(json.dumps(out, indent=2))
sys.exit(1 if out['failed'] else 0)
