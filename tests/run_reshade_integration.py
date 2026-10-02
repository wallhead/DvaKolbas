"""Isolated production-presenter check using an explicitly supplied ReShade DLL."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

parser = argparse.ArgumentParser()
parser.add_argument('--exe', type=Path, required=True)
parser.add_argument('--runtime', type=Path, required=True)
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('--fsr-runtime-dir', type=Path)
args = parser.parse_args()
args.output.mkdir(parents=True, exist_ok=True)
records = []
(args.output / 'results.json').unlink(missing_ok=True)
routes = ['--ordinary-reshade', '--require-reshade']
if args.fsr_runtime_dir:
    routes.append('--fsr-reshade')
for route in routes:
    with tempfile.TemporaryDirectory(prefix='trp-reshade-', dir=args.output.resolve()) as temporary:
        root = Path(temporary)
        shutil.copy2(args.exe, root / args.exe.name)
        shutil.copy2(args.runtime, root / 'dxgi.dll')
        runtime_records = []
        if route == '--fsr-reshade':
            pins = Path(__file__).parent.parent / 'tools' / 'fsr'
            for pin_name in ('runtime-pin.json', 'fg-runtime-pin.json'):
                for entry in json.loads((pins / pin_name).read_text())['runtime']:
                    dll = args.fsr_runtime_dir / entry['filename']
                    digest = hashlib.sha256(dll.read_bytes()).hexdigest()
                    if digest != entry['sha256'] or dll.stat().st_size != entry['bytes']:
                        raise RuntimeError(f"Pinned FSR runtime mismatch: {dll}")
                    (root / 'FSR').mkdir(exist_ok=True)
                    shutil.copy2(dll, root / 'FSR' / dll.name)
                    runtime_records.append({'filename': dll.name, 'sha256': digest})
        fixture = Path(__file__).parent / 'fixtures' / 'reshade'
        shutil.copytree(fixture, root, dirs_exist_ok=True)
        (root / 'screenshots').mkdir(exist_ok=True)
        result = subprocess.run([str(root / args.exe.name), route], cwd=root,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=120)
        log = args.output / (route.removeprefix('--') + '.log')
        log.write_bytes(result.stdout)
        print(result.stdout.decode('utf-8', errors='replace'), flush=True)
        if result.returncode:
            raise SystemExit(result.returncode)
        records.append({'route': route, 'result': 'PASS', 'fsrRuntime': runtime_records})
(args.output / 'results.json').write_text(json.dumps({
    'runtimeSha256': hashlib.sha256(args.runtime.read_bytes()).hexdigest(),
    'routes': records, 'skyrimTested': False}, indent=2) + '\n')
