#!/usr/bin/env python3
"""Verify the exact supplied build and recorded landmarks without executing DLLs.

Usage: python verify_evidence.py EXTRACTED_ROOT --archive INPUT.7z --output verification.json
No third-party Python dependencies. A failed check exits nonzero.
"""
from __future__ import annotations
import argparse, hashlib, json, pathlib, platform, struct, sys
from pe_index import PE
from pdb_index import PDB
from pdb_types import Types


def sha256(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open('rb') as f:
        for block in iter(lambda: f.read(1 << 20), b''):
            h.update(block)
    return h.hexdigest()


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('extracted_root', type=pathlib.Path)
    ap.add_argument('--archive', type=pathlib.Path)
    ap.add_argument('--output', type=pathlib.Path)
    args = ap.parse_args()
    evidence = pathlib.Path(__file__).resolve().parent.parent / 'evidence'
    manifest = json.loads((evidence / 'manifest.json').read_text())
    verified_files = []
    for record in manifest['files']:
        file = args.extracted_root / record['path']
        require(file.is_file(), f'Missing file: {file}')
        require(file.stat().st_size == record['bytes'], f'Size mismatch: {file}')
        require(sha256(file) == record['sha256'], f'Hash mismatch: {file}')
        verified_files.append(record['path'])
    if args.archive:
        require(args.archive.stat().st_size == manifest['archive']['bytes'], 'Archive size mismatch')
        require(sha256(args.archive) == manifest['archive']['sha256'], 'Archive hash mismatch')
    modules = {}
    records = json.loads((evidence / 'landmarks.json').read_text())
    file_map = {pathlib.PurePosixPath(r['path']).name: r['path'] for r in manifest['files']}
    for r in records:
        name = r['module']
        if name not in modules:
            modules[name] = PE(args.extracted_root / file_map[name])
        pe = modules[name]
        require(hashlib.sha256(pe.data).hexdigest() == r['sha256'], f'Wrong landmark module: {name}')
        expected = bytes.fromhex(r['bytes'])
        require(pe.read(int(r['rva'], 0), len(expected)) == expected,
                f'Instruction mismatch: {name}+{r["rva"]}')
    identities = []
    for stem, prefix in [('amd_fidelityfx_framegeneration_dx12', 'amd_fg'),
                         ('amd_fidelityfx_loader_dx12', 'amd_loader')]:
        pe = PE(args.extracted_root / 'UpscalerBasePlugin' / (stem + '.dll'))
        pdb = PDB(args.extracted_root / 'UpscalerBasePlugin' / (stem + '.pdb'))
        stored = json.loads((evidence / (prefix + '.identity.json')).read_text())
        require(pdb.identity['guid'] == stored['guid'] and pdb.identity['age'] == stored['age'],
                f'PDB identity changed: {stem}')
        require(any(x['guid'] == pdb.identity['guid'] and x['age'] == pdb.identity['age']
                    for x in pe.codeview()), f'PDB/DLL mismatch: {stem}')
        identities.append({'module': stem, **pdb.identity})
    fg_pdb = PDB(args.extracted_root / 'UpscalerBasePlugin/amd_fidelityfx_framegeneration_dx12.pdb')
    types = Types(fg_pdb)
    stored_types = json.loads((evidence / 'amd_fg_struct_layouts.json').read_text())
    nfields = 0
    for record in stored_types:
        actual = types.struct_info(record['type_index'])
        require(actual['name'] == record['name'] and actual['size'] == record['size'],
                f'Type mismatch: {record["name"]}')
        fields = types.fields(actual['field_list'])
        require(fields == record['fields'], f'Field mismatch: {record["name"]}')
        nfields += len(fields)
    pd = modules['PDPerfPlugin.dll']
    fg = modules['amd_fidelityfx_framegeneration_dx12.dll']
    vtable_checks = [(pd, 0x11A82D8+8, 0xD0CB0),
                     (pd, 0x11A9F28+8, 0xEF230),
                     (fg, 0x1073E8+0x160, 0x94A0),
                     (fg, 0x1073E8+0x168, 0x98E0)]
    for pe, slot, target in vtable_checks:
        require(pe.ptr(slot)-pe.base == target, f'Vtable mismatch: {pe.path.name}+{slot:x}')
    host = modules['SkyrimUpscaler.dll']
    require(len(host.functions) == 10058 and host.invalid_functions == 0, 'Host exception directory mismatch')
    # Negative control for the equality gate, without altering any user file.
    negative_control = False
    try:
        require(b'\x00' == b'\x01', 'Expected deliberate mismatch')
    except ValueError:
        negative_control = True
    result = {
        'status': 'PASS', 'scope': 'Static input, byte, PDB and descriptor reproducibility only; no GPU or game test',
        'archive_checked': bool(args.archive), 'files_hashed': len(verified_files),
        'instruction_landmarks_checked': len(records), 'pdb_identities': identities,
        'types_reparsed': len(stored_types), 'fields_reparsed': nfields,
        'vtable_slots_checked': len(vtable_checks), 'host_valid_exception_entries': len(host.functions),
        'negative_comparison_control': negative_control, 'python': platform.python_version(),
        'verified_files': verified_files,
    }
    text = json.dumps(result, indent=2)
    if args.output:
        args.output.write_text(text+'\n')
    print(text)
    return 0

if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (ValueError, OSError, KeyError, struct.error) as exc:
        print(f'VERIFICATION FAILED: {exc}', file=sys.stderr)
        raise SystemExit(1)
