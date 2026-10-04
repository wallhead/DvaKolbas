"""Disassemble bounded preparation witnesses in the exact AIO19 PD image."""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import capstone
import pefile

PD_SHA = 'ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435'
RANGES = [(0xa5fd0, 0x30f), (0xa0a70, 0x22), (0xa0aa0, 0x20),
          (0xa1e00, 0x210), (0xa2740, 0x125)]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    data = args.binary.read_bytes()
    if hashlib.sha256(data).hexdigest() != PD_SHA:
        raise ValueError('Unqualified PD image; no offsets applied')
    pe = pefile.PE(data=data)
    dis = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    result = dict(binarySha256=PD_SHA, capstoneVersion=capstone.__version__,
                  toolSha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                  scope='Static instruction witnesses only; no executed-branch or GPU claim',
                  constants={hex(rva): struct.unpack('<f', pe.get_data(rva, 4))[0]
                             for rva in (0x12fc098, 0x12fc0e0)}, ranges=[])
    for rva, size in RANGES:
        code = pe.get_data(rva, size)
        lines = [dict(rva=hex(i.address-pe.OPTIONAL_HEADER.ImageBase),
                      bytes=i.bytes.hex(), mnemonic=i.mnemonic, operands=i.op_str)
                 for i in dis.disasm(code, pe.OPTIONAL_HEADER.ImageBase+rva)]
        result['ranges'].append(dict(rva=hex(rva), requestedBytes=size,
                                     codeSha256=hashlib.sha256(code).hexdigest(), instructions=lines))
    args.output.write_text(json.dumps(result, indent=2) + '\n')
    print(f'Traced {len(RANGES)} pinned ranges; static evidence only')


if __name__ == '__main__':
    main()
