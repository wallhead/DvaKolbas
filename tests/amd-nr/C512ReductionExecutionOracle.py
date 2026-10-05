"""Independent packed GPU reduction oracle, generated half inputs and padding."""
import argparse
import json
import pathlib
import struct
import subprocess
import numpy as np
from NumericOracle import encode_e4m3_distance


def packed_addresses(width, height):
    # Forward gather equations, independently of the shader's inverse address.
    x, y, n = np.indices((width, height, 512), dtype=np.uint32)
    q = ((x % 4) * 4 + y % 4) * 512 + n
    a, b = (q << 4) & 0x1e00, q & 31
    c = ((q >> 3) & 0x3c0) | (q & 3)
    d = np.minimum(b, (b - np.uint32(16)) & 0xffff) >> 2
    correction = np.where(q > 4095, -508, 0)
    inner = 16*d + ((c.astype(np.int64) + a.astype(np.int64) + correction) | np.where(b > 15, 8, 0))
    return (((x // 4) * (height // 4) + y // 4) * 8192 + inner).ravel()


def expected_output(source, initial, width, height):
    active_w, active_h = min(source.shape[0] // 2, width), min(source.shape[1] // 2, height)
    v = source.view(np.float16)
    with np.errstate(all='ignore'):
        left = (v[0:2*active_w:2, 0:2*active_h:2] + v[0:2*active_w:2, 1:2*active_h:2]).astype(np.float16)
        right = (v[1:2*active_w:2, 0:2*active_h:2] + v[1:2*active_w:2, 1:2*active_h:2]).astype(np.float16)
        total = (left + right).astype(np.float16)
        average = (total * np.float16(0.25)).astype(np.float16)
        encoded = encode_e4m3_distance(average.astype(np.float32).ravel().view(np.uint32)).astype(np.uint8)
    addresses = packed_addresses(width, height).reshape(width, height, 512)
    assert len(np.unique(addresses)) == initial.size and int(addresses.max()) == initial.size - 1
    result = initial.copy()
    result[addresses[:active_w, :active_h].ravel()] = encoded
    return result, encoded


def fixtures():
    rng = np.random.default_rng(20261005)
    quads = np.zeros((131072, 4), dtype=np.uint16)
    quads[:65536, 0] = np.arange(65536, dtype=np.uint16)
    quads[65536:] = rng.integers(0, 65536, (65536, 4), dtype=np.uint16)
    literals = np.array([
        [0x6400, 0x3800, 0xe400, 0x3800], [0x8000]*4, [0x7c00, 0, 0xfc00, 0],
        [0x7bff]*4, [0x7d01, 0, 0, 0], [1]*4, [0x3c00]*4, [0xbc00]*4,
        [0, 0x8000, 0x8000, 0x8000], [0x1400]*4, [0x1401]*4], dtype=np.uint16)
    quads[65536:65547] = literals
    spatial = quads.reshape(32, 8, 512, 4)
    source = np.empty((64, 16, 512), dtype=np.uint16)
    source[0::2, 0::2] = spatial[:, :, :, 0];source[0::2, 1::2] = spatial[:, :, :, 1]
    source[1::2, 0::2] = spatial[:, :, :, 2];source[1::2, 1::2] = spatial[:, :, :, 3]
    yield 'all-half-tuples', source, 32, 8, rng.integers(0, 256, 32*8*512, dtype=np.uint8)
    for name, sw, sh, dw, dh in [('padded', 60, 36, 32, 20), ('cropped', 16, 12, 4, 4), ('maximum', 64, 64, 32, 32)]:
        yield name, rng.integers(0, 65536, (sw, sh, 512), dtype=np.uint16), dw, dh, rng.integers(0, 256, dw*dh*512, dtype=np.uint8)
    # All coordinates vary independently; asymmetric axes/channel values pin association.
    x, y, n = np.indices((8, 16, 512), dtype=np.uint32)
    source = ((x * 997 + y * 61 + n * 7) % 0x7c00).astype(np.uint16)
    yield 'asymmetric', source, 4, 8, rng.integers(0, 256, 4*8*512, dtype=np.uint8)


def main():
    parser = argparse.ArgumentParser();parser.add_argument('--exe', required=True);parser.add_argument('--work-dir', required=True)
    mode = parser.add_mutually_exclusive_group();mode.add_argument('--warp', action='store_true');mode.add_argument('--adapter-luid')
    args = parser.parse_args();root = pathlib.Path(args.work_dir);root.mkdir(parents=True, exist_ok=True)
    target = ['--adapter-luid', args.adapter_luid] if args.adapter_luid else ['--warp'] if args.warp else ['--cpu']
    total, active_total, padding_total = 0, 0, 0
    for name, source, dw, dh, initial in fixtures():
        expected, encoded = expected_output(source, initial, dw, dh)
        if name == 'all-half-tuples':
            assert list(encoded[65536:65547]) == [0x20, 0x80, 0x7f, 0x7e, 0x7f, 0, 0x38, 0xb8, 0, 0, 1]
        if name == 'padded':
            assert initial.size - encoded.size == 51200
        fixture = struct.pack('<IIII', source.shape[0], source.shape[1], dw, dh) + source.astype('<u2').tobytes() + initial.tobytes()
        inp, out = root/(name+'.input.bin'), root/(name+'.output.bin');inp.write_bytes(fixture)
        (root/(name+'.expected.bin')).write_bytes(expected.tobytes())
        run = subprocess.run([args.exe, '--oracle', *target, str(inp), str(out)], capture_output=True, text=True)
        if run.returncode: raise RuntimeError(run.stdout + run.stderr)
        (root/(name+'.execution.txt')).write_text(run.stdout + run.stderr)
        actual = np.frombuffer(out.read_bytes(), dtype=np.uint8)
        assert actual.size == expected.size, (actual.size, expected.size)
        different = np.flatnonzero(actual != expected)
        assert not different.size, [(int(i), int(actual[i]), int(expected[i])) for i in different[:10]]
        for suffix, bad in [('short', fixture[:-1]), ('tail', fixture+b'x'),
                            ('source-cap', struct.pack('<IIII', 68, 64, dw, dh)+fixture[16:]),
                            ('destination-cap', struct.pack('<IIII', 4, 4, 36, 32)+fixture[16:]),
                            ('zero', struct.pack('<IIII', 0, 4, dw, dh)+fixture[16:])]:
            rejected_in = root/(name+'-'+suffix+'.input.bin');rejected_in.write_bytes(bad)
            preserved = root/(name+'-rejected.output.bin');preserved.write_bytes(b'preserve')
            fail = subprocess.run([args.exe, '--oracle', *target, str(rejected_in), str(preserved)], capture_output=True)
            assert fail.returncode != 0 and preserved.read_bytes() == b'preserve'
        total += actual.size;active_total += encoded.size;padding_total += actual.size-encoded.size
        print(f'PASS {name}: {actual.size} packed bytes ({actual.size-encoded.size} preserved padding)')
    report = {'target': target, 'cases': 5, 'bytes': total, 'active_values': active_total, 'padding_bytes': padding_total, 'mismatches': 0}
    (root/'report.json').write_text(json.dumps(report, indent=2)+'\n');print(json.dumps(report))


if __name__ == '__main__': main()
