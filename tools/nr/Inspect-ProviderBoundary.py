"""Offline string/RIP-reference candidates in a pinned provider PE.

No process loading, memory patching, callbacks or inferred resource ownership.
Candidate function intervals come from PE unwind metadata, not another version.
"""
import argparse
import bisect
import hashlib
import json
import re
from pathlib import Path
import capstone
import pefile


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("binary", type=Path)
    p.add_argument("output", type=Path)
    p.add_argument("--expected-sha256", required=True)
    a = p.parse_args()
    with a.binary.open("rb") as f:
        digest = hashlib.file_digest(f, "sha256").hexdigest()
    if digest != a.expected_sha256:
        raise SystemExit("Provider hash mismatch; no candidate scan performed")
    pe = pefile.PE(str(a.binary))
    wanted = {"DLSSG.OutputInterpolated", "DLSSG.OutputReal", "DLSSG.HUDLess", "DLSSG.UI",
              "DLSSG.Depth", "DLSSG.MVecs", "presentFrame", "presentCommon",
              "event.cmdBuf.present-interpolated", "event.cmdBuf.present-real",
              "slHookPresent", "slHookPresent1"}
    strings = {}
    for m in re.finditer(rb"[\x20-\x7e]{6,}\x00", pe.__data__):
        value = m.group()[:-1].decode("ascii")
        if value in wanted:
            strings[pe.get_rva_from_offset(m.start())] = value
    functions = sorted((e.struct.BeginAddress, e.struct.EndAddress)
                       for e in getattr(pe, "DIRECTORY_ENTRY_EXCEPTION", ()))
    starts = [f[0] for f in functions]
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    decoder.detail = True
    decoder.skipdata = True
    refs = []
    base = pe.OPTIONAL_HEADER.ImageBase
    for section in pe.sections:
        if not section.Characteristics & 0x20000000:
            continue
        for insn in decoder.disasm(section.get_data(), base + section.VirtualAddress):
            if not insn.id:
                continue
            for operand in insn.operands:
                if operand.type == capstone.CS_OP_MEM and operand.mem.base == capstone.x86.X86_REG_RIP:
                    target = insn.address + insn.size + operand.mem.disp - base
                    if target not in strings:
                        continue
                    rva = insn.address - base
                    index = bisect.bisect_right(starts, rva) - 1
                    interval = functions[index] if index >= 0 and rva < functions[index][1] else None
                    refs.append({"string": strings[target], "stringRva": hex(target), "instructionRva": hex(rva),
                                 "instructionBytes": insn.bytes.hex(), "instruction": f"{insn.mnemonic} {insn.op_str}",
                                 "unwindFunction": [hex(v) for v in interval] if interval else None})
    output = {"schema": 1, "binary": str(a.binary.resolve()), "sha256": digest,
              "capstone": capstone.__version__, "pefile": pefile.__version__, "references": refs,
              "qualification": "Static candidates only. No observed generated outputs, guide times, state or retirement contract."}
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    print(f"pinned strings={len(strings)} RIP references={len(refs)}; no hooks installed")


if __name__ == "__main__":
    main()
