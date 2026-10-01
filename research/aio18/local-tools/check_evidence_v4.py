"""Compare supplied v4 excerpt instructions with the matching local host bytes."""
import hashlib
import json
import pathlib
import re

import capstone
import pefile

root = pathlib.Path(__file__).resolve().parents[1]
review = root / 'evidence-review-v4'
host = root / 'aio-build18/SKSE/Plugins/SkyrimUpscaler.dll'
sha = hashlib.sha256(host.read_bytes()).hexdigest()
assert sha == '5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81'
pe = pefile.PE(str(host))
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
base = pe.OPTIONAL_HEADER.ImageBase

def normalize(op):
    op = op.split(' ; ', 1)[0].split('#', 1)[0].strip().lower()
    def number(match):
        n = int(match[0], 16)
        if (1 << 31) <= n < (1 << 32):
            n -= 1 << 32
        elif n >= (1 << 63):
            n -= 1 << 64
        return str(n)
    op = re.sub(r'0x[0-9a-f]+', number, op)
    op = re.sub(r'\*1(?=[+\]])', '', re.sub(r'\s+', '', op))
    return re.sub(r'((?:cs|ds|es|fs|gs|ss):)\[([0-9]+)\]', r'\1\2', op)

witnesses = []
by_address = {}
for path in sorted((review / 'evidence/v4/disassembly').glob('*.asm')):
    source = path.read_text(encoding='utf-8')
    assert f'SHA256={sha}' in source
    count = 0
    for line in source.splitlines():
        m = re.match(r'^([0-9A-F]{8}):\s+(\S+)\s*(.*)$', line)
        if not m:
            continue
        rva, mnemonic, operands = int(m[1], 16), m[2], m[3]
        while mnemonic in ('rex', 'rex.W', 'data16', 'cs'):
            tokens = operands.split(None, 1)
            mnemonic = tokens[0]
            operands = tokens[1] if len(tokens) > 1 else ''
        instruction = next(decoder.disasm(pe.get_data(rva, 15), base + rva, count=1))
        expected_mnemonic = {'movabs': 'mov'}.get(mnemonic, mnemonic)
        actual_mnemonic = {'movabs': 'mov'}.get(instruction.mnemonic, instruction.mnemonic)
        nop_alias = mnemonic == 'xchg' and normalize(operands) == 'ax,ax' and instruction.bytes == bytes.fromhex('6690')
        if nop_alias:
            expected_mnemonic = 'nop'
        assert expected_mnemonic == actual_mnemonic, (path.name, hex(rva), mnemonic, instruction.mnemonic)
        if mnemonic != 'nop' and not nop_alias:
            assert normalize(operands) == normalize(instruction.op_str), (path.name, hex(rva), operands, instruction.op_str)
        witness = {
            'module': 'SkyrimUpscaler.dll', 'sha256': sha, 'rva': hex(rva),
            'bytes': instruction.bytes.hex(), 'instruction': f'{instruction.mnemonic} {instruction.op_str}',
            'evidence': str(path.relative_to(review)).replace('\\', '/'),
        }
        witnesses.append(witness)
        by_address[rva] = witness
        count += 1
    assert count > 0

selected = [
    (0x19ec61, 'Context hook installation region'),
    (0x284ab2, 'Register init_effect_runtime event 9'),
    (0x284ada, 'Register destroy_effect_runtime event 10'),
    (0x284b02, 'Register open-overlay event 86'),
    (0x284b2a, 'Register begin-effects event 76'),
    (0x284b52, 'Register finish-effects event 77'),
    (0x284b76, 'Register reshade_present event 75'),
    (0x284ec0, 'Begin-effects routing flag bracket'),
    (0x284e70, 'Finish-effects routing flag bracket'),
    (0x19fcf3, 'UI target binding before phase enable'),
    (0x19fd00, 'Normal UI phase enable'),
    (0x1a0827, 'StatsMenu UI phase clear'),
    (0x21350e, 'ImGui scissor call'),
    (0x2135ac, 'ImGui scissor restore call'),
    (0x299750, 'ENB presence probe'),
    (0x2a97e0, 'ReShade-aware target routing wrapper'),
]
records = []
for rva, label in selected:
    assert rva in by_address, hex(rva)
    records.append({**by_address[rva], 'label': label})
result = {
    'status': 'PASS', 'scope': 'Local static comparison of v4 excerpt instructions, not runtime traces or whole-program completeness',
    'host_sha256': sha, 'excerpt_files_checked': 8, 'excerpt_instruction_records_compared': len(witnesses),
    'comparison': 'Capstone mnemonic/operand comparison; equivalent REX/padding, signed immediates, unit index scale and segment notation normalized; nop operands excluded',
    'local_host_routing_landmarks': len(records),
    'global_direct_scissor_search_rerun': False,
    'note': 'The archive global-search claim remains supplied evidence; local excerpts confirm the two identified calls.',
}
(review / 'local-v4-verification.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
(review / 'local-host-routing-landmarks.json').write_text(json.dumps(records, indent=2), encoding='utf-8')
(review / 'local-v4-instruction-witnesses.json').write_text(json.dumps(witnesses, indent=2), encoding='utf-8')
print(json.dumps(result, indent=2))
