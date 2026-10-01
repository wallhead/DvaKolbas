import csv
import hashlib
import io
import json
import pathlib
import re
import subprocess
import zipfile

root = pathlib.Path(__file__).resolve().parents[2]
def staged(path):
    return subprocess.check_output(['git', 'show', ':' + path], cwd=root)

paths = subprocess.check_output(['git', 'diff', '--cached', '--name-only', '-z'], cwd=root).decode().split('\0')[:-1]
assert all(p == '.gitattributes' or p.startswith('research/') or p in ['docs/RE_DATABASE.md', 'docs/FSR_AIO_RE_NOTES.md', 'docs/FSR_IMPLEMENTATION.md', 'docs/superpowers/plans/2026-10-01-fsr-sr.md'] for p in paths)
assert not any(pathlib.Path(p).suffix.lower() in {'.dll', '.exe', '.pdb', '.sqlite', '.db'} for p in paths)
exact = 0
for p in paths:
    if p.startswith(('research/aio18/supplied-v2/', 'research/aio18/supplied-v3/', 'research/aio18/supplied-v4/', 'research/aio18/local-analysis/', 'research/aio18/local-tools/', 'research/dlss-baseline/', 'research/references/', 'research/aio18/source-bundles/')):
        assert staged(p) == (root / p).read_bytes(), p
        exact += 1

checksum_count = 0
cache_entries = 0
for revision in (2, 3, 4):
    base = f'research/aio18/supplied-v{revision}/'
    checksums = staged(base + 'SHA256SUMS.txt').decode().splitlines()
    for row in checksums:
        digest, name = row.split('  ', 1)
        name = name.removeprefix('./')
        if '__pycache__' in pathlib.PurePosixPath(name).parts or name.endswith('.pyc'):
            assert revision == 4
            with zipfile.ZipFile(io.BytesIO(staged('research/aio18/source-bundles/AIO18_FSR_RE_Evidence_v4.zip'))) as z:
                data = z.read('AIO18_FSR_RE_v4/' + name)
            cache_entries += 1
            assert not (root / base / name).exists()
        else:
            data = staged(base + name)
        assert hashlib.sha256(data).hexdigest() == digest, name
    checksum_count += len(checksums)

for name, digest in [
    ('AIO18_FSR_RE_Evidence.zip', '51478598f0027fd729f41ca4a186c66c303d7b2356c26ca1edd9b82c614f5beb'),
    ('AIO18_FSR_RE_Evidence_v2.zip', '6c713e119831097ebdce14ffa99aaf76dc6c153276619f9608ace3b8578206ae'),
    ('AIO18_FSR_RE_Evidence_v3.zip', 'ca9e0b41ffd58dbfdae5f97cf0c3296479f43a79daa6b2e7f2fd25b172b1c62b'),
    ('AIO18_FSR_RE_Evidence_v4.zip', '25f00214e2251e3a682692ddf634930e7cbb2e392becea9623b618e226edd096'),
]:
    data = staged('research/aio18/source-bundles/' + name)
    assert hashlib.sha256(data).hexdigest() == digest
    with zipfile.ZipFile(io.BytesIO(data)) as z:
        assert not any(pathlib.PurePosixPath(p).suffix.lower() in {'.dll', '.exe', '.pdb'} for p in z.namelist())

addresses = list(csv.DictReader(io.StringIO(staged('research/aio18/address-map.csv').decode())))
modules = list(csv.DictReader(io.StringIO(staged('research/aio18/modules.csv').decode())))
assert len(addresses) == 325
assert len({(r['module'], int(r['rva'], 0)) for r in addresses}) == 316
assert len(modules) == 21
hashes = {r['module']: r['sha256'] for r in modules}
assert all(hashes[r['module']] == r['module_sha256'] for r in addresses)
routing = list(csv.DictReader(io.StringIO(staged('research/aio18/host-routing-map.csv').decode())))
assert len(routing) == 16
assert len({(r['module'], int(r['rva'], 0)) for r in routing}) == 16
assert all(hashes[r['module']] == r['module_sha256'] for r in routing)
for r in addresses + routing:
    assert (root / 'research/aio18' / r['evidence']).is_file()
    assert r['validation'] == 'static bytes checked locally; runtime not tested'

for name in ['docs/RE_DATABASE.md', 'research/README.md']:
    for target in re.findall(r'\]\(([^)]+)\)', (root / name).read_text(encoding='utf-8')):
        if '://' not in target:
            assert (root / name).parent.joinpath(target).resolve().exists(), (name, target)

assert cache_entries == 7
assert not any(pathlib.Path(p).suffix.lower() == '.pyc' for p in paths)

check = subprocess.run(['git', 'diff', '--cached', '--check'], cwd=root, capture_output=True)
assert check.returncode == 0, check.stdout[:500]
result = {
    'status': 'PASS', 'scope': 'Research publication integrity; no new FSR runtime test',
    'staged_files_checked': len(paths), 'archived_files_byte_identical_to_working_copies': exact,
    'supplied_v2_v3_v4_checksum_entries_checked': checksum_count,
    'omitted_v4_cache_entries_verified_inside_original_staged_zip': cache_entries,
    'original_evidence_archives_checked': 4, 'address_records': 325, 'distinct_module_addresses': 316,
    'module_records': 21, 'additional_host_routing_records': 16, 'local_index_links_checked': True,
    'runtime_binaries_or_sql_database_in_staged_files': False, 'git_whitespace_check': 'PASS',
}
(root / 'out/research/publication-checks.json').write_text(json.dumps(result, indent=2) + '\n')
print(json.dumps(result, indent=2))
