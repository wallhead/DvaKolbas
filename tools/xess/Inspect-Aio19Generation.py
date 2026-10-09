"""Read-only AIO19 Intel FG/XeLL call-site evidence; never loads a DLL.

Requires pefile and Capstone. Only the two pinned Build19-Hotfix1 callers are
accepted. Addresses are RVAs, not live-process addresses. Unwind entries bound
disassembly fragments; they are not automatically complete C++ functions.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

PINS = {
    "UpscalerBasePlugin/PDPerfPlugin.dll":
        (20332032, "ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435"),
    "SKSE/Plugins/SkyrimUpscaler.dll":
        (15975424, "ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a"),
}
# Extra instruction windows provide the struct stores and timing evidence which
# are too far from their API call to appear in the short call-site context.
WINDOWS = {
    "constructor-vtable": (0x84D50, 0x84D70, 3),
    "requested-frame-limit": (0x84F50, 0x850A3, 8),
    "sleep-parameters": (0x84F50, 0x85114, 6),
    "ui-tag-description": (0x85920, 0x85DC8, 22),
    "motion-tag-description": (0x85920, 0x85E4A, 25),
    "depth-tag-description": (0x85920, 0x85EE6, 28),
    "render-time-delta": (0x85920, 0x85F8E, 4),
    "frame-constants-fields": (0x85920, 0x86144, 15),
    "init-flags-ui-mode": (0x866B0, 0x86783, 30),
    "next-frame-sleep": (0x87340, 0x874CD, 13),
    "qpc-milliseconds": (0x836A0, 0x83704, 12),
    "generic-fg-export": (0x116410, 0x11647B, 6),
    "d11-controller-vtable": (0x110440, 0x11045B, 3),
    "d11-controller-evaluate": (0x112760, 0x11281D, 3),
    "transport-xefg-construction": (0x107480, 0x1076E6, 13),
}


class Evidence:
    def __init__(self, path, pin):
        raw = path.read_bytes()
        digest = hashlib.sha256(raw).hexdigest()
        if (len(raw), digest) != pin:
            raise ValueError(f"Pinned artifact mismatch: {path.name}")
        self.identity = {"bytes": len(raw), "sha256": digest}
        self.pe = pefile.PE(data=raw)
        self.base = self.pe.OPTIONAL_HEADER.ImageBase
        self.decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
        self.decoder.detail = True
        self.cache = {}
        self.imports = {}
        for kind in ("DIRECTORY_ENTRY_IMPORT", "DIRECTORY_ENTRY_DELAY_IMPORT"):
            for entry in getattr(self.pe, kind, []):
                for symbol in entry.imports:
                    if symbol.name:
                        self.imports[symbol.address - self.base] = {
                            "dll": entry.dll.decode(), "api": symbol.name.decode(),
                            "delay": kind == "DIRECTORY_ENTRY_DELAY_IMPORT",
                        }

    def fragment(self, rva):
        entry = next((e.struct for e in self.pe.DIRECTORY_ENTRY_EXCEPTION
                      if e.struct.BeginAddress <= rva < e.struct.EndAddress), None)
        if entry is None:
            raise ValueError(f"No unwind fragment at {rva:#x}")
        start, end = entry.BeginAddress, entry.EndAddress
        if start not in self.cache:
            instructions = list(self.decoder.disasm(self.pe.get_data(start, end-start), self.base+start))
            if not instructions or instructions[-1].address + instructions[-1].size != self.base+end:
                raise ValueError(f"Incomplete disassembly of {start:#x}..{end:#x}")
            self.cache[start] = instructions
        return start, end, self.cache[start]

    def encode(self, instructions):
        return [{"rva": hex(i.address-self.base), "bytes": i.bytes.hex(),
                 "text": i.mnemonic + " " + i.op_str} for i in instructions]

    def window(self, fragment, rva, count):
        start, end, ins = self.fragment(fragment)
        index = next(n for n, i in enumerate(ins) if i.address-self.base == rva)
        return {"fragment": [hex(start), hex(end)], "instructions": self.encode(ins[index:index+count])}

    def calls(self, wanted):
        result = []
        for section in self.pe.sections:
            if not section.Characteristics & 0x20000000:
                continue
            code, pos = section.get_data(), 0
            while (pos := code.find(b"\xff\x15", pos)) >= 0:
                offset = pos
                pos += 1
                if offset+6 > len(code):
                    continue
                rva = section.VirtualAddress+offset
                slot = rva+6+struct.unpack_from("<i", code, offset+2)[0]
                imported = self.imports.get(slot)
                if not imported or not wanted(imported):
                    continue
                start, end, ins = self.fragment(rva)
                index = next((n for n, i in enumerate(ins)
                              if i.address-self.base == rva and i.mnemonic == "call"), None)
                if index is None:
                    continue  # A byte-pattern hit is not an instruction witness.
                result.append({**imported, "iatRva": hex(slot), "callRva": hex(rva),
                               "fragment": [hex(start), hex(end)],
                               "instructions": self.encode(ins[max(0,index-9):index+4])})
        return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--extracted", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    perf, game = (Evidence(args.extracted/name, pin) for name, pin in PINS.items())
    calls = perf.calls(lambda item: item["dll"] in ("libxess_fg.dll", "libxell.dll"))
    if len(calls) != 33:
        raise ValueError(f"Unexpected pinned SDK call-site count: {len(calls)}")
    support = {name: perf.window(*window) for name, window in WINDOWS.items()}
    # Validate the FG class vtable and D11 controller dispatch table as data.
    tables = {}
    for table, offsets in ((0x12EEF10, (8, 0x10, 0x20, 0x28, 0x30, 0x68, 0x70)),
                           (0x12FACF0, (0x20, 0x40))):
        tables[hex(table)] = {hex(o): hex(struct.unpack("<Q", perf.pe.get_data(table+o,8))[0]-perf.base)
                              for o in offsets}
    multiplier = struct.unpack("<d", perf.pe.get_data(0x12FC190,8))[0]
    if multiplier != 1000.0:
        raise ValueError("Unexpected QPC clock conversion")
    result = {
        "scope": "Static, hash-pinned delay-import and instruction witnesses; no DLL execution",
        "sdkReferenceCommit": "8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0",
        "callers": {name: e.identity for name, e in zip(PINS, (perf, game))},
        "sdkCalls": calls, "supportingWindows": support, "vtableSlots": tables,
        "qpcMultiplier": {"rva": "0x12fc190", "double": multiplier},
        "gameFgCalls": game.calls(lambda item: item["api"] == "EvaluateFrameGeneration"),
        "limitations": ["Static calls do not prove execution, hardware capability or visual quality.",
                        "No explicit UI-composition enable was found in the inspected FG wrapper.",
                        "The outer controller to selected transport assignment is not fully traced.",
                        "No complete SDK-reader retirement or pre-input timing proof is claimed."],
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2)+"\n", encoding="utf-8")
    print(f"Verified {len(calls)} FG/XeLL calls; {len(support)} supporting windows; "
          f"{len(result['gameFgCalls'])} game FG dispatch call(s)")


if __name__ == "__main__":
    main()
