"""Local v3 byte/range checks using pefile and Capstone; no target DLL execution."""
import hashlib
import json
import pathlib
import re
import struct

import capstone
import pefile

root = pathlib.Path(__file__).resolve().parents[1]
review = root / 'evidence-review-v3'
def load(name):
    return json.loads((review / name).read_text(encoding='utf-8'))
files = {pathlib.PurePosixPath(r['path']).name: root / 'aio-build18' / r['path']
         for r in load('evidence/manifest.json')['files']}
modules = {}
def module(name):
    if name not in modules:
        modules[name] = pefile.PE(str(files[name]))
    return modules[name]

decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
line_count = 0
spans = load('evidence/v3/excerpt_manifest_v3.json')
for span in spans:
    data = (review / span['file']).read_bytes()
    assert hashlib.sha256(data).hexdigest() == span['file_sha256']
    pe = module(span['module'])
    assert hashlib.sha256(files[span['module']].read_bytes()).hexdigest() == span['module_sha256']
    raw = pe.get_data(span['start_rva'], span['end_rva'] - span['start_rva'])
    assert hashlib.sha256(raw).hexdigest() == span['byte_range_sha256']
    decoded = list(decoder.disasm(raw, span['start_rva']))
    assert sum(i.size for i in decoded) == len(raw), span['file']
    boundaries = {i.address for i in decoded}
    addresses = [int(a, 16) for a in re.findall(r'^([0-9A-F]{8}):', data.decode(), re.M)]
    assert len(addresses) == span['instruction_lines']
    assert all(a in boundaries for a in addresses), span['file']
    line_count += len(addresses)

constants = load('evidence/v3/constants_and_imports.json')
for row in constants:
    raw = bytes.fromhex(row['bytes'])
    assert module(row['module']).get_data(row['rva'], len(raw)) == raw

vtable = load('evidence/v3/pd_api_dimension_vtable.json')
pd = module('PDPerfPlugin.dll')
for entry in vtable['entries']:
    ptr = struct.unpack('<Q', pd.get_data(vtable['vtable_rva'] + entry['offset'], 8))[0]
    assert ptr - pd.OPTIONAL_HEADER.ImageBase == entry['target_rva']
marks = []
for revision, name in [(1, 'landmarks.json'), (2, 'v2/landmarks_v2.json'), (3, 'v3/landmarks_v3.json')]:
    marks.extend(load('evidence/' + name))
result = {
    'status': 'PASS', 'scope': 'Local v3 static byte, span, and instruction-boundary checks; no GPU or semantic-completeness proof',
    'new_code_spans_checked': len(spans), 'excerpt_instruction_boundaries_checked': line_count,
    'constant_records_checked': len(constants), 'pd_api_vtable_slots_checked': len(vtable['entries']),
    'cumulative_instruction_records': len(marks),
    'distinct_module_addresses': len({(m['module'], int(m['rva'], 0)) for m in marks}),
    'gnu_llvm_verifier_rerun': False,
    'note': 'Capstone checked boundaries locally; the supplied GNU/LLVM verification remains a supplied result.',
}
(review / 'local-v3-verification.json').write_text(json.dumps(result, indent=2))
print(json.dumps(result, indent=2))
