"""Independent serial-order C512 diagnostic oracle. Uses only generated records."""
import argparse
import json
import pathlib
import struct
import subprocess
import numpy as np
from NumericOracle import encode_e4m3_distance, compare_words


def fp8_table():
    codes = np.arange(256, dtype=np.uint32)
    e, m = (codes >> 3) & 15, codes & 7
    values = np.where(e == 0, m.astype(np.float64) / 512,
                      np.ldexp(1 + m.astype(np.float64) / 8, e.astype(np.int32) - 7))
    values[codes >= 128] *= -1
    values[(codes & 127) == 127] = np.nan
    return values.astype(np.float32)


def half_round(values):
    with np.errstate(all='ignore'):
        bits = values.astype(np.float16).view(np.uint16)
    bits[np.isnan(values)] = 0x7e00
    return bits


def evaluate(input_codes, residual, matrix, coefficients):
    decode = fp8_table()
    with np.errstate(all='ignore'):
        c = coefficients.view(np.float16).astype(np.float32)
        initial = half_round(np.add(decode[residual] * c, np.float32(0), dtype=np.float32))
        accumulator = initial.copy()
        decoded_input, decoded_matrix = decode[input_codes], decode[matrix]
        for chunk in range(16):
            dot = np.zeros(input_codes.shape, dtype=np.float32)
            for k in range(chunk * 32, (chunk + 1) * 32):
                dot = np.add(dot, decoded_input[:, k, None] * decoded_matrix[None, :, k], dtype=np.float32)
            accumulator = half_round(np.add(dot, accumulator.view(np.float16).astype(np.float32), dtype=np.float32))
        encoded = encode_e4m3_distance(accumulator.view(np.float16).astype(np.float32).ravel().view(np.uint32))
    output = np.empty((input_codes.size, 2), dtype='<u4')
    output[:, 0] = accumulator.ravel().astype(np.uint32) | (encoded << 16)
    output[:, 1] = initial.ravel()
    return output


def raw_record(matrix, coefficients):
    # Independent forward native-fragment address, not the product's bit-deposit inverse.
    n, k = np.indices((512, 512), dtype=np.uint32)
    lane = 16 * ((k % 16) // 8) + n % 16
    v2 = lane >> 1
    address = ((((v2 & 6) | (lane & 1)) << 6) | ((v2 & 8) << 2)) + ((lane << 2) & 8)
    address += (n // 16) * 512 + (k // 32) * 0x4000 + (((k % 16) // 4) % 2) * 16 + ((k // 16) % 2) * 4 + k % 4
    assert len(np.unique(address)) == 262144
    raw = np.zeros(263168, dtype=np.uint8)
    raw[address.ravel()] = matrix.ravel()
    for thread in range(256):
        for group in range(thread >> 5, 32, 8):
            channel = 16 * group | (thread & 15)
            at = (262144 + (((channel & 0x1f1) | ((thread << 2) & 8)) << 1)) | (thread & 12)
            raw[at] = int(coefficients[channel]) & 255
            raw[at + 1] = int(coefficients[channel]) >> 8
    return raw.tobytes()


def fixtures():
    matrix = np.zeros((512, 512), dtype=np.uint8)
    k = np.arange(512);matrix[(37 * k + 11) % 512, k] = 0x38
    p = np.arange(16)[:, None]
    inputs = (((p + k) % 126 + 1) | (((p + k) & 1) << 7)).astype(np.uint8)
    yield 'permutation', 4, 4, inputs, np.zeros_like(inputs), matrix, np.zeros(512, dtype=np.uint16)
    rng = np.random.default_rng(20261005)
    finite = lambda shape: (rng.integers(0, 81, shape, dtype=np.uint8) | (rng.integers(0, 2, shape, dtype=np.uint8) << 7))
    coef = rng.uniform(-2, 2, 512).astype(np.float16).view(np.uint16)
    yield 'finite-nonsquare', 4, 8, finite((32, 512)), finite((32, 512)), finite((512, 512)), coef
    inputs = np.ones((16, 512), dtype=np.uint8);residual = np.zeros_like(inputs)
    matrix = np.zeros((512, 512), dtype=np.uint8);matrix[0, :] = 1;matrix[0, 0] = 0x7e
    coef = np.zeros(512, dtype=np.uint16)
    yield 'chunk-rounding', 4, 4, inputs, residual, matrix, coef
    matrix = np.zeros((512, 512), dtype=np.uint8);matrix[0, 0] = 0xb8
    inputs = np.zeros((16, 512), dtype=np.uint8);inputs[:, 0] = 0x39
    residual = inputs.copy();coef = np.zeros(512, dtype=np.uint16);coef[0] = 0x3c01
    yield 'early-residual', 4, 4, inputs, residual, matrix, coef
    matrix = np.zeros((512, 512), dtype=np.uint8);matrix[511, 0] = 0x7f
    inputs = np.zeros((16, 512), dtype=np.uint8);residual = np.full_like(inputs, 0x80)
    residual[:, :4] = [0, 0x38, 0xb8, 0x38]
    coef = np.full(512, 0x3c00, dtype=np.uint16);coef[:4] = [0x7c00, 0x7c00, 0x7c00, 0x7d01]
    yield 'special', 4, 4, inputs, residual, matrix, coef


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True);parser.add_argument('--work-dir', required=True)
    mode = parser.add_mutually_exclusive_group();mode.add_argument('--warp', action='store_true');mode.add_argument('--adapter-luid')
    args = parser.parse_args();work = pathlib.Path(args.work_dir);work.mkdir(parents=True, exist_ok=True)
    target = ['--adapter-luid', args.adapter_luid] if args.adapter_luid else ['--warp'] if args.warp else ['--cpu']
    total = 0
    for name, width, height, inputs, residual, matrix, coef in fixtures():
        expected = evaluate(inputs, residual, matrix, coef)
        if name == 'chunk-rounding':
            assert np.all(expected[::512, 0] == 0x00363b00)
        elif name == 'early-residual':
            assert np.all(expected[::512] == [0x1400, 0x3c81])
        elif name == 'special':
            assert np.all(expected.reshape(16, 512, 2)[:, :4, 0] == [0x007f7e00, 0x007e7c00, 0x00fefc00, 0x007f7e00])
        fixture = struct.pack('<II', width, height) + inputs.tobytes() + residual.tobytes() + raw_record(matrix, coef)
        source, output = work / (name + '.input.bin'), work / (name + '.output.bin')
        source.write_bytes(fixture)
        (work / (name + '.expected.bin')).write_bytes(expected.tobytes())
        command = [args.exe, '--oracle', *target, str(source), str(output)]
        run = subprocess.run(command, capture_output=True, text=True)
        if run.returncode:
            raise RuntimeError(run.stdout + run.stderr)
        (work / (name + '.execution.txt')).write_text(run.stdout + run.stderr)
        compare_words(output.read_bytes(), expected.tobytes());total += expected.size
        # A rejected fixture must not truncate an already existing diagnostic output.
        for suffix, bad in [('short', fixture[:-1]), ('tail', fixture + b'x'), ('extent', struct.pack('<II', 20, 16) + fixture[8:])]:
            invalid = work / (name + '-' + suffix + '.input.bin');invalid.write_bytes(bad)
            preserved = work / (name + '-rejected.output.bin');preserved.write_bytes(b'preserve')
            rejected = subprocess.run([args.exe, '--oracle', *target, str(invalid), str(preserved)], capture_output=True)
            assert rejected.returncode != 0 and preserved.read_bytes() == b'preserve'
        print(f'PASS {name}: {expected.size} diagnostic words')
    report = {'target': target, 'cases': 5, 'values': total // 2, 'words': total, 'mismatches': 0,
              'order': 'ascending K, FP32 serial sum per 32 terms; explicit half round after residual and each chunk'}
    (work / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report))


if __name__ == '__main__':
    main()
