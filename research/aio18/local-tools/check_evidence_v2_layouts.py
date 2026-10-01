import hashlib
import json
import pathlib
import struct
import sys

import pefile

root = pathlib.Path(__file__).resolve().parents[1]
review = pathlib.Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else root / 'evidence-review-v2'
sys.path.insert(0, str(review / 'scripts'))
# These uploaded readers were inspected before this local import. They only parse bytes.
from pdb_index import PDB
from pdb_class_layout import ClassTypes

def load(name):
    return json.loads((review / name).read_text(encoding='utf-8'))

extracted = root / 'aio-build18'
manifest = load('evidence/manifest.json')
files = {pathlib.PurePosixPath(x['path']).name: extracted / x['path'] for x in manifest['files']}
modules = {}
def module(name):
    if name not in modules:
        modules[name] = pefile.PE(str(files[name]))
    return modules[name]

spans = load('evidence/v2/excerpt_manifest_v2.json')
for span in spans:
    assert hashlib.sha256((review / span['file']).read_bytes()).hexdigest() == span['file_sha256']
    raw = module(span['module']).get_data(span['start_rva'], span['end_rva'] - span['start_rva'])
    assert hashlib.sha256(raw).hexdigest() == span['byte_range_sha256'], span['file']

fg = module('amd_fidelityfx_framegeneration_dx12.dll')
vtables = load('evidence/v2/amd_swapchain_vtable.json')
for slot in vtables:
    actual = struct.unpack('<Q', fg.get_data(slot['vtable_rva'] + slot['offset'], 8))[0]
    assert actual - fg.OPTIONAL_HEADER.ImageBase == slot['target_rva']

pdb = PDB(files['amd_fidelityfx_framegeneration_dx12.pdb'])
types = ClassTypes(pdb)
layouts = load('evidence/v2/amd_internal_class_layouts.json')
for layout in layouts:
    assert types.layout(layout['type_index']) == layout, layout['name']
plain = load('evidence/amd_fg_struct_layouts.json')
for layout in plain:
    actual = types.layout(layout['type_index'])
    assert actual['name'] == layout['name'] and actual['size'] == layout['size']
    assert [{k: f[k] for k in ('name', 'offset', 'type', 'type_index')} for f in actual['fields']] == layout['fields']

types.rec[0x7ffffffe] = (0x1203, bytes.fromhex('11110000'))
try:
    types.class_fields(0x7ffffffe)
except ValueError:
    rejected = True
else:
    raise AssertionError('Unknown leaf accepted')
result = {
    'status': 'PASS', 'scope': 'Local byte-span/vtable checks and rerun of reviewed uploaded PDB reader; no GPU validation',
    'new_code_spans_checked': len(spans), 'amd_vtable_slots_checked': len(vtables),
    'new_class_layouts_reparsed': len(layouts), 'new_direct_fields_reparsed': sum(len(x['fields']) for x in layouts),
    'original_struct_layouts_reparsed': len(plain), 'original_fields_reparsed': sum(len(x['fields']) for x in plain),
    'unknown_leaf_rejected': rejected,
    'gnu_objdump_verifier_rerun': False,
    'note': f"Capstone checked all {load('independent-verification.json')['landmarks_hashed_and_decoded']} instruction byte records in independent-verification.json; objdump is unavailable locally.",
}
(review / 'local-layout-verification.json').write_text(json.dumps(result, indent=2))
print(json.dumps(result, indent=2))
