"""Hash-gated AIO19 HDR witnesses. Reads files; never loads or executes a DLL.

Requires pefile and Capstone. RVAs and interpretations apply only to the pinned
Build19-Hotfix1 archive. These witnesses do not qualify HDR output in gameplay.
"""
import argparse
import hashlib
import json
from pathlib import Path

import capstone
import pefile

PINS = {
    'SKSE/Plugins/SkyrimUpscaler.dll': (15975424, 'ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a'),
    'SKSE/Plugins/SkyrimUpscaler.ini': (15300, 'ad0b46779903a9fec921a3fedb4f03c84eb393fd646ba889e5875608a6979967'),
    'UpscalerBasePlugin/PDPerfPlugin.dll': (20332032, 'ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435'),
}
ARCHIVE = (204042400, '49e7f7dabf426937915d1aeed664fc40a7cc7d89f42092a69c205b22c4687439')
STRINGS = {
    'SKSE/Plugins/SkyrimUpscaler.dll': {
        0x469c28: 'mDLSSNRColorIsHdr',
        0x45aa58: 'Treats the NR input color as HDR when using Ratio resolve.',
        0x46a620: 'HDR::UiTexture', 0x46a698: 'HDR::HdrTexture',
        0x46a720: 'CS UI route: {} (observed at UI boundary; HDR resources present={})',
    },
    'UpscalerBasePlugin/PDPerfPlugin.dll': {
        0x12f7a20: 'DXGISwapChainProxy: Creating Fake HDR Buffer DXGI_FORMAT_R16G16B16A16_FLOAT',
        0x12f7d00: 'DXGISwapChainProxy::ResizeBuffers: Creating Fake HDR Buffer DXGI_FORMAT_R16G16B16A16_FLOAT',
    },
}
WITNESSES = {
    'SKSE/Plugins/SkyrimUpscaler.dll': [
        (0x2f3596, '4c8d058b661700', 'INI reader references mDLSSNRColorIsHdr', 0x469c28),
        (0x2f35ae, '8886e4020000', 'INI boolean return is stored at settings +0x2e4', None),
        (0x2ecd18, '488d1501d91700', 'Named HDR UI texture lookup', 0x46a620),
        (0x2ecd2b, '488998b0160000', 'Successful UI texture match retained at host +0x16b0', None),
        (0x2ecd32, '488d155fd91700', 'Named HDR scene texture lookup', 0x46a698),
        (0x2ecd48, '488998b8160000', 'Successful HDR texture match retained at host +0x16b8', None),
        (0x282f10, '48ffa030010000', 'SetColorSpace1 forwards through retained receiver vtable +0x130; method identity inferred from labelled function and DXGI layout', None),
    ],
    'UpscalerBasePlugin/PDPerfPlugin.dll': [
        (0xfe474, '488d0da5951f01', 'Conditional FP16 fake-HDR-buffer branch label', 0x12f7a20),
        (0xfe49d, 'c745c80a000000', 'Descriptor format written as DXGI value 10 (R16G16B16A16_FLOAT)', None),
        (0x101dae, '488d0d4b5f1f01', 'Resize FP16 fake-HDR-buffer branch label', 0x12f7d00),
        (0x101e0a, 'c745d80a000000', 'Resize descriptor format written as DXGI value 10', None),
    ],
}


def identity(path, expected):
    with path.open('rb') as stream:
        digest = hashlib.file_digest(stream, 'sha256').hexdigest()
    actual = (path.stat().st_size, digest)
    if actual != expected:
        raise ValueError(f'Artifact identity mismatch: {path.name}')
    return {'bytes': actual[0], 'sha256': actual[1]}


def inspect(args):
    report = {'scope': 'Static PE strings, raw bytes and unwind-bounded Capstone instructions; no DLL execution',
              'archive': identity(args.archive, ARCHIVE), 'artifacts': {},
              'dependencies': {'pefile': pefile.__version__, 'capstone': capstone.__version__}}
    for name, pin in PINS.items():
        artifact = identity(args.extracted / name, pin)
        if name.endswith('.dll'):
            pe = pefile.PE(str(args.extracted / name))
            base = pe.OPTIONAL_HEADER.ImageBase
            decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
            decoder.detail = True
            for rva, value in STRINGS[name].items():
                if pe.get_data(rva, len(value) + 1) != value.encode('ascii') + b'\0':
                    raise ValueError(f'String mismatch: {name} RVA={rva:#x}')
            artifact['strings'] = {hex(k): v for k, v in STRINGS[name].items()}
            artifact['witnesses'] = []
            for rva, expected, interpretation, target in WITNESSES[name]:
                owner = next(e.struct for e in pe.DIRECTORY_ENTRY_EXCEPTION
                             if e.struct.BeginAddress <= rva < e.struct.EndAddress)
                instructions = list(decoder.disasm(pe.get_data(owner.BeginAddress,
                                                               owner.EndAddress-owner.BeginAddress),
                                                  base+owner.BeginAddress))
                index = next(i for i, ins in enumerate(instructions) if ins.address-base == rva)
                ins = instructions[index]
                if ins.bytes.hex() != expected:
                    raise ValueError(f'Instruction mismatch: {name} RVA={rva:#x}')
                if target is not None:
                    mem = next(op.mem for op in ins.operands
                               if op.type == capstone.x86.X86_OP_MEM and op.mem.base == capstone.x86.X86_REG_RIP)
                    if ins.address + ins.size + mem.disp - base != target:
                        raise ValueError(f'RIP target mismatch: {name} RVA={rva:#x}')
                artifact['witnesses'].append({'rva': hex(rva), 'interpretation': interpretation,
                    'unwindFragment': [hex(owner.BeginAddress), hex(owner.EndAddress)],
                    'instructions': [{'rva': hex(i.address-base), 'bytes': i.bytes.hex(),
                                      'text': f'{i.mnemonic} {i.op_str}'}
                                     for i in instructions[max(0, index-2):index+4]]})
        else:
            text = (args.extracted / name).read_text(encoding='utf-8-sig')
            if '#Tell NR the input color is HDR.\nmDLSSNRColorIsHdr = false' not in text:
                raise ValueError('INI input-HDR setting/comment mismatch')
            artifact['inputHdrDefault'] = False
        report['artifacts'][name] = artifact
    report['notEstablished'] = ['AIO19 ENB SDR-to-HDR10 conversion', 'AIO19 live HDR calibration or toggle',
        'Live HDR swapchain/transfer/luminance values', 'HDR correctness with FSR3/FSR4 FG',
        'Full dataflow of the NR input-HDR flag beyond its INI storage and UI description']
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print('PASS: archive, 3 artifacts, 7 strings and 11 instruction witnesses; static only')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--archive', type=Path, required=True)
    parser.add_argument('--extracted', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    inspect(parser.parse_args())
