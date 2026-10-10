"""Read-only, hash-pinned AIO19 XeSS Present callback/timing witnesses.

Requires pefile and Capstone, as does Inspect-Aio19Generation.py. Does not load,
patch or execute either DLL. Leaf ranges below are explicit byte witnesses,
not guessed unwind fragments. Static paths do not establish thread ownership.
"""
import argparse
import importlib.util
import json
import struct
from pathlib import Path

support = importlib.util.spec_from_file_location(
    "aio19_generation", Path(__file__).with_name("Inspect-Aio19Generation.py"))
generation = importlib.util.module_from_spec(support)
support.loader.exec_module(generation)

WINDOWS = {
    "method-3-construction": (0x107480, 0x1076E6, 13),
    "attach-selected-transport": (0x107810, 0x10782F, 10),
    "install-present-callbacks": (0x107810, 0x1078AA, 13),
    "outer-vtable-install": (0xFE620, 0xFE66E, 3),
    "present-ordinary-route": (0x100AD0, 0x100CEB, 7),
    "synchronous-preparation-route": (0xFF240, 0xFF556, 5),
    "prepare-then-present": (0xFF5F0, 0xFF885, 19),
    "invoke-pre-callback": (0x101470, 0x1017CD, 10),
    "inner-present": (0x101870, 0x1018F4, 5),
    "invoke-post-callback": (0x101870, 0x1019AA, 10),
    "pre-callback-virtual-dispatch": (0x107750, 0x1077B0, 12),
    "evaluation-fallback-call": (0x85920, 0x85C36, 3),
    "present-fallback-markers": (0x86FD0, 0x87270, 17),
    "post-present-status": (0x87340, 0x873DE, 4),
    "next-frame-sleep": (0x87340, 0x874CD, 13),
}
LEAVES = {
    "post-callback": (0x1077F0,
        "448bc9488b0df6e526014885c9740d488b014c8bc2418bd148ff6070c3"),
    "present1-forward": (0xA9A70, "488b0148ff6040"),
}


def fallback_callers(evidence):
    """Scan E8 candidates, then require a decoded unwind instruction boundary."""
    sites = []
    for section in evidence.pe.sections:
        if not section.Characteristics & 0x20000000:
            continue
        code, pos = section.get_data(), 0
        while (pos := code.find(b"\xe8", pos)) >= 0:
            offset, pos = pos, pos + 1
            if offset + 5 > len(code):
                continue
            rva = section.VirtualAddress + offset
            target = rva + 5 + struct.unpack_from("<i", code, offset + 1)[0]
            if target != 0x85670:
                continue
            start, end, instructions = evidence.fragment(rva)
            index = next((n for n, i in enumerate(instructions)
                          if i.address - evidence.base == rva and i.mnemonic == "call"), None)
            if index is not None:
                sites.append({"callRva": hex(rva), "fragment": [hex(start), hex(end)],
                              "instructions": evidence.encode(instructions[max(0, index-3):index+3])})
    if sorted(int(site["callRva"], 16) for site in sites) != [0x85C39, 0x87273]:
        raise ValueError("Unexpected fallback caller witnesses")
    return sites


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--extracted", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    name = "UpscalerBasePlugin/PDPerfPlugin.dll"
    evidence = generation.Evidence(args.extracted / name, generation.PINS[name])
    windows = {label: evidence.window(*window) for label, window in WINDOWS.items()}
    leaves = {}
    for label, (rva, expected) in LEAVES.items():
        raw = bytes.fromhex(expected)
        if evidence.pe.get_data(rva, len(raw)) != raw:
            raise ValueError(f"Leaf byte witness mismatch: {label}")
        instructions = list(evidence.decoder.disasm(raw, evidence.base + rva))
        if sum(i.size for i in instructions) != len(raw):
            raise ValueError(f"Incomplete leaf disassembly: {label}")
        leaves[label] = {"range": [hex(rva), hex(rva+len(raw))],
                         "instructions": evidence.encode(instructions)}
    tables = {}
    for table, slots in {0x12EEF10: {0x68: 0x86FD0, 0x70: 0x87340},
                         0x12F8110: {0x40: 0x100AD0, 0xB0: 0xA9A70}}.items():
        actual = {offset: struct.unpack("<Q", evidence.pe.get_data(table+offset, 8))[0]
                  - evidence.base for offset in slots}
        if actual != slots:
            raise ValueError(f"Vtable witness mismatch: {table:#x}")
        tables[hex(table)] = {hex(offset): hex(value) for offset, value in actual.items()}
    result = {
        "scope": "Static hash-pinned Present dispatch and timing; no DLL execution",
        "caller": {"path": name, **evidence.identity},
        "windows": windows, "leaves": leaves, "vtableSlots": tables,
        "fallbackDirectCallers": fallback_callers(evidence),
        "limitations": [
            "Inspected ordinary synchronous Present route; worker/cross-adapter branches remain unqualified.",
            "Post callback arguments are buffer index/resource, not the Present HRESULT.",
            "No genuine Skyrim input boundary, executing thread or GPU-reader drain is established.",
            "Two direct fallback callers do not establish absence of indirect callers elsewhere.",
        ],
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2)+"\n", encoding="utf-8")
    print(f"Verified {len(windows)} windows, {len(leaves)} leaves, 4 vtable slots "
          "and 2 decoded fallback direct callers")


if __name__ == "__main__":
    main()
