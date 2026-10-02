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
args = parser.parse_args()
args.output.mkdir(parents=True, exist_ok=True)
records = []
for route in ('--ordinary-reshade', '--require-reshade'):
    with tempfile.TemporaryDirectory(prefix='trp-reshade-', dir=args.output.resolve()) as temporary:
        root = Path(temporary)
        shutil.copy2(args.exe, root / args.exe.name)
        shutil.copy2(args.runtime, root / 'dxgi.dll')
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
        records.append({'route': route, 'result': 'PASS'})
(args.output / 'results.json').write_text(json.dumps({
    'runtimeSha256': hashlib.sha256(args.runtime.read_bytes()).hexdigest(),
    'routes': records, 'skyrimTested': False}, indent=2) + '\n')
