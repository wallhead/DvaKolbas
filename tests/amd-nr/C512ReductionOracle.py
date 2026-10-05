"""Independent CPU half-reduction oracle; generated inputs, no trained data."""
import argparse
from pathlib import Path
import subprocess

import numpy as np
from NumericOracle import encode_e4m3_distance


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--work-dir', required=True)
    args = parser.parse_args()
    root = Path(args.work_dir)
    root.mkdir(parents=True, exist_ok=True)
    quads = np.zeros((131072, 4), dtype=np.uint16)
    quads[:65536, 0] = np.arange(65536, dtype=np.uint16)
    quads[65536:] = np.random.default_rng(20261005).integers(0, 65536, (65536, 4), dtype=np.uint16)
    literals = np.array([
        [0x6400, 0x3800, 0xe400, 0x3800],
        [0x8000]*4,
        [0x7c00, 0, 0xfc00, 0],
        [0x7bff]*4,
        [0x7d01, 0, 0, 0],
        [0x0001]*4,
        [0x3c00]*4,
        [0xbc00]*4,
        [0, 0x8000, 0x8000, 0x8000],
        [0x1000]*4,  # zero/FP8 midpoint boundary
        [0x1001]*4,
    ], dtype=np.uint16)
    quads[65536:65536+len(literals)] = literals
    values = quads.view(np.float16)
    with np.errstate(all='ignore'):
        left = (values[:, 0] + values[:, 1]).astype(np.float16)
        right = (values[:, 2] + values[:, 3]).astype(np.float16)
        total = (left + right).astype(np.float16)
        average = (total * np.float16(0.25)).astype(np.float16)
        expected = encode_e4m3_distance(average.astype(np.float32).view(np.uint32)).astype(np.uint8)
    assert list(expected[65536:65545]) == [0x20, 0x80, 0x7f, 0x7e, 0x7f, 0, 0x38, 0xb8, 0]
    # Canonical source [X,Y,channel], with Y advancing before X.
    source = np.empty((64, 16, 512), dtype=np.uint16)
    spatial = quads.reshape(32, 8, 512, 4)
    source[0::2, 0::2] = spatial[:, :, :, 0]
    source[0::2, 1::2] = spatial[:, :, :, 1]
    source[1::2, 0::2] = spatial[:, :, :, 2]
    source[1::2, 1::2] = spatial[:, :, :, 3]
    input_path, output_path = root/'input-half.bin', root/'output-e4m3.bin'
    input_path.write_bytes(source.astype('<u2').tobytes())
    subprocess.run([args.exe, '--oracle', str(input_path), str(output_path)], check=True)
    actual = np.frombuffer(output_path.read_bytes(), dtype=np.uint8)
    assert actual.size == expected.size, (actual.size, expected.size)
    mismatches = np.flatnonzero(actual != expected)
    assert not mismatches.size, [(int(i), int(actual[i]), int(expected[i])) for i in mismatches[:10]]
    print(f'PASS: {actual.size} half-reduction outputs, zero E4M3 byte mismatches; all half words and seeded tuples')


if __name__ == '__main__':
    main()
