"""Validate saved real Intel qualification evidence without rerunning the GPU."""
import argparse
import hashlib
import json
import re
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--receipt', type=Path, required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    data = json.loads(args.receipt.read_text(encoding='utf-8-sig'))
    assert data['qualified'] and data['exitCode'] == 0 and not data['numericalOnly']
    assert data['visualUserConfirmation'] == 'Smoother; HUD intact'
    pins = json.loads((root / 'tools/xess/sdk-pin.json').read_text())
    assert data['sdkCommit'] == pins['commit']
    for runtime in pins['generationRuntimes']:
        assert data['runtimeHashes'][Path(runtime['stagePath']).name] == runtime['sha256']
    for path, expected in data['sourceFilesSHA256'].items():
        assert hashlib.sha256((root / path).read_bytes()).hexdigest() == expected, path
    raw = args.receipt.with_suffix('.log').read_bytes()
    log = raw.decode('utf-16' if raw.startswith((b'\xff\xfe', b'\xfe\xff')) else 'utf-8-sig')
    assert data['summary'] in log
    assert 'visual_requested=1 interrupted=0 lost_focus_frames=0' in data['summary']
    assert 'errors=0 clean_retirement=1' in data['summary']
    frames = re.findall(r'FRAME id=(\d+) phase=(\d+) focus=(\d+) tag=(\d+) generate=(\d+) sdk_frames=(\d+) enabled=(\d+) result=(-?\d+)', log)
    for phase in (1, 3):
        assert sum(int(p) == phase and g == '1' and count == '2' and result == '0' for _, p, _, _, g, count, _, result in frames) >= 3
    for _, phase, focus, _, generate, count, _, result in frames:
        assert focus == '1' and int(result) >= 0
        if phase in ('0', '2') or generate == '0':
            assert count == '1'
    pixels = re.findall(r'PIXEL x=(\d+) y=(\d+) actual=([0-9a-f]+) expected=([0-9a-f]+)', log)
    assert len(pixels) == 6 and all(actual == expected for _, _, actual, expected in pixels)
    assert {actual for _, _, actual, _ in pixels} == {'ff505050', 'ff0000ff', 'ff28a828'}
    print('PASS: real Intel 2-frame status in both on phases, off/menu passthrough, opaque/translucent HUD readbacks, visual confirmation and clean retirement; saved source hashes match')


if __name__ == '__main__':
    main()
