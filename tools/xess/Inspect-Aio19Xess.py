"""Read-only, hash-gated AIO19 XeSS inventory, loader and dispatch witnesses.

The runtime members are streamed from 7z; no additional large DLL copies and
no DLL loading/execution are needed. Requires pefile and Capstone.
"""
import argparse
import hashlib
import json
import struct
import subprocess
from pathlib import Path

import capstone
import pefile

SDK_COMMIT = "8fe81bdbbaf00b3c1b733fd0d830c333dc84e6f0"
ARCHIVE_PIN = (204042400, "49e7f7dabf426937915d1aeed664fc40a7cc7d89f42092a69c205b22c4687439")
PE_PINS = {
    "SKSE/Plugins/SkyrimUpscaler.dll": (15975424, "ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a"),
    "UpscalerBasePlugin/PDPerfPlugin.dll": (20332032, "ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435"),
}
# Git object identities of Intel's unmodified bin/ payload at the pinned tag.
SDK_BLOBS = {
    "libxess.dll": (77795704, "548837c12b215d189884d54db0a480cd3bf2bfd1"),
    "libxess_dx11.dll": (156016, "1e0247ba23a841ff514841ff963b72b636802a4e"),
    "libxess_fg.dll": (22957432, "57a5da469d79e8dceea6af01961030219d391cdf"),
    "libxell.dll": (415368, "c321c6c6cd148dc614e6a339a27976ce0874d2e7"),
}
API_STRINGS = {
    0x12FBD08: "xessD3D11CreateContext",
    0x12FBD48: "xessD3D11Execute",
    0x12FBD60: "xessD3D12CreateContext",
    0x12FBD38: "xessD3D12Init",
    0x12FBDD0: "xessD3D12Execute",
    0x12EE9F0: "xefgSwapChainSetNumInterpolatedFrames",
}


def identify(path, pin):
    with path.open("rb") as stream:
        digest = hashlib.file_digest(stream, "sha256").hexdigest()
    if (path.stat().st_size, digest) != pin:
        raise ValueError(f"Pinned artifact mismatch: {path.name}")
    return {"bytes": pin[0], "sha256": digest}


def runtime_member(args, name, pin):
    proc = subprocess.Popen([str(args.sevenzip), "e", "-so", "-bd", "-bso0", "-bsp0",
                             str(args.archive), f"UpscalerBasePlugin/{name}"],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    sha256 = hashlib.sha256()
    blob = hashlib.sha1(f"blob {pin[0]}\0".encode())
    size = 0
    while chunk := proc.stdout.read(1024 * 1024):
        size += len(chunk)
        sha256.update(chunk)
        blob.update(chunk)
    proc.stdout.close()
    error = proc.stderr.read().decode(errors="replace")
    proc.stderr.close()
    if proc.wait() or (size, blob.hexdigest()) != pin:
        raise ValueError(f"Archive member/official SDK identity mismatch: {name}; {error[:200]}")
    return {"bytes": size, "sha256": sha256.hexdigest(), "gitBlobSha1": blob.hexdigest(),
            "matchesOfficial302": True}


def loader_witnesses(path):
    pe = pefile.PE(str(path))
    base = pe.OPTIONAL_HEADER.ImageBase
    imports = {s.address - base: s.name.decode() for e in pe.DIRECTORY_ENTRY_IMPORT
               for s in e.imports if s.name}
    for rva, name in API_STRINGS.items():
        if pe.get_data(rva, len(name) + 1) != name.encode() + b"\0":
            raise ValueError(f"Pinned API string mismatch: {name}")
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    decoder.detail = True
    result = []
    # Bounded PE-unwind fragments containing the previously located RIP xrefs.
    for start in (0x84F50, 0xB5050, 0x11F310):
        fragment = next(e.struct for e in pe.DIRECTORY_ENTRY_EXCEPTION if e.struct.BeginAddress == start)
        ins = list(decoder.disasm(pe.get_data(start, fragment.EndAddress - start), base + start))
        for index, instruction in enumerate(ins):
            for op in instruction.operands:
                if op.type != capstone.x86.X86_OP_MEM or op.mem.base != capstone.x86.X86_REG_RIP:
                    continue
                target = instruction.address + instruction.size + op.mem.disp - base
                if target not in API_STRINGS:
                    continue
                following = ins[index + 1:index + 7]
                call = next(i for i in following if i.mnemonic == "call")
                operand = call.operands[0]
                if operand.type != capstone.x86.X86_OP_MEM or operand.mem.base != capstone.x86.X86_REG_RIP:
                    raise ValueError("Unexpected symbol resolver call")
                resolver = call.address + call.size + operand.mem.disp - base
                if imports.get(resolver) != "GetProcAddress":
                    raise ValueError("API name is not passed to GetProcAddress")
                result.append({"api": API_STRINGS[target], "stringRva": hex(target),
                               "fragment": [hex(start), hex(fragment.EndAddress)],
                               "resolver": "GetProcAddress", "instructions": [
                                   {"rva": hex(i.address - base), "bytes": i.bytes.hex(),
                                    "text": f"{i.mnemonic} {i.op_str}"}
                                   for i in ins[max(0, index - 2):index + 7]]})
    if len(result) != 7:
        raise ValueError(f"Expected seven loader witnesses, found {len(result)}")
    return result


def sr_dispatch_witness(path):
    """Verify the API-table chain into a static D3D12 execute call site."""
    pe = pefile.PE(str(path))
    base = pe.OPTIONAL_HEADER.ImageBase
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    decoder.detail = True

    def fragment(start):
        entry = next(e.struct for e in pe.DIRECTORY_ENTRY_EXCEPTION if e.struct.BeginAddress == start)
        return list(decoder.disasm(pe.get_data(start, entry.EndAddress - start), base + start))

    constructor = fragment(0x11F880)
    execute = fragment(0x120070)
    loader = fragment(0x11F310)
    by_rva = {i.address - base: i for i in constructor + execute + loader}

    def rip_target(rva):
        i = by_rva[rva]
        operand = next(o for o in i.operands if o.type == capstone.x86.X86_OP_MEM)
        if operand.mem.base != capstone.x86.X86_REG_RIP:
            raise ValueError("Unexpected API-table address operand")
        return i.address + i.size + operand.mem.disp - base

    table = rip_target(0x11F9B2)
    holder = rip_target(0x11F9B9)
    slot = rip_target(0x11F854)  # store immediately after the pinned Execute resolver
    if (table, holder, slot) != (0x137AB50, 0x1375E40, 0x137AC20):
        raise ValueError("Unexpected XeSS API-table chain")
    if rip_target(0x120135) != holder or slot - table != 0xD0:
        raise ValueError("D3D12 dispatch does not select the pinned Execute slot")
    expected = {0x12013C: ("mov", "rbx, qword ptr [rax + 0xd0]"),
                0x120148: ("lea", "r8, [rbp - 0x39]"),
                0x12014C: ("mov", "rdx, r14"),
                0x12014F: ("mov", "rcx, qword ptr [rax]"),
                0x120152: ("call", "rbx")}
    for rva, (mnemonic, operands) in expected.items():
        i = by_rva[rva]
        if (i.mnemonic, i.op_str) != (mnemonic, operands):
            raise ValueError(f"Unexpected D3D12 dispatch instruction at {rva:x}")
    method = int.from_bytes(pe.get_data(0x12FBFF8, 8), "little") - base
    if method != 0x11FF30:
        raise ValueError("Unexpected XeSS execute virtual-method entry")
    return {"api": "xessD3D12Execute", "methodEntryRva": hex(method),
            "vtableEntryRva": "0x12fbff8", "apiTableRva": hex(table),
            "apiTableHolderRva": hex(holder), "resolvedSlotRva": hex(slot),
            "callRva": "0x120152", "parameterBase": "rbp-0x39",
            "instructions": [{"rva": hex(i.address - base), "bytes": i.bytes.hex(),
                              "text": f"{i.mnemonic} {i.op_str}"} for i in execute],
            "limits": "Static dispatch and field stores, not a runtime capture; texture provenance, formats and upstream conversions remain unverified."}


def game_input_witnesses(path):
    """Pin the game-side public interface calls and jitter arithmetic."""
    pe = pefile.PE(str(path))
    base = pe.OPTIONAL_HEADER.ImageBase
    imports = {s.address - base: s.name.decode() for e in pe.DIRECTORY_ENTRY_DELAY_IMPORT
               if e.dll == b"PDPerfPlugin.dll" for s in e.imports if s.name}
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    decoder.detail = True
    ranges = ((0x1B484A, 0x1B48CC, 0x1B493F),
              (0x2E952A, 0x2E96A8, 0x2E970F),
              (0x2E9802, 0x2E97FA, 0x2E9842),
              (0x2F8E76, 0x2F8F2E, 0x2F8F78),
              (0x2F935A, 0x2F95BB, 0x2F95F5))
    fragments = []
    instructions = {}
    for start, low, high in ranges:
        entry = next(e.struct for e in pe.DIRECTORY_ENTRY_EXCEPTION if e.struct.BeginAddress == start)
        decoded = list(decoder.disasm(pe.get_data(start, entry.EndAddress - start), base + start))
        instructions.update({i.address - base: i for i in decoded})
        selected = [i for i in decoded if low <= i.address - base < high]
        fragments.append({"fragment": [hex(start), hex(entry.EndAddress)],
                          "instructions": [{"rva": hex(i.address - base), "bytes": i.bytes.hex(),
                                            "text": f"{i.mnemonic} {i.op_str}"} for i in selected]})

    def rip_target(i):
        op = next(o for o in i.operands if o.type == capstone.x86.X86_OP_MEM)
        if op.mem.base != capstone.x86.X86_REG_RIP:
            raise ValueError("Unexpected game-side witness operand")
        return i.address + i.size + op.mem.disp - base

    calls = []
    for rva, api in ((0x1B48CC, "GetJitterOffset"), (0x2E983C, "EvaluateUpscaler"),
                     (0x2F95E2, "SetMotionScaleX"), (0x2F95EF, "SetMotionScaleY")):
        i = instructions[rva]
        slot = rip_target(i)
        if i.mnemonic != "call" or imports.get(slot) != api:
            raise ValueError(f"Unexpected game-side API call: {api}")
        calls.append({"api": api, "callRva": hex(rva), "delayImportSlotRva": hex(slot)})
    constants = []
    for rva, expected in ((0x1B48EB, 0xC0000000), (0x1B4903, 0x40000000),
                          (0x1B490B, 0x80000000), (0x1B4928, 0x80000000)):
        location = rip_target(instructions[rva])
        value = struct.unpack("<I", pe.get_data(location, 4))[0]
        if value != expected:
            raise ValueError(f"Unexpected jitter constant at {rva:x}")
        constants.append({"instructionRva": hex(rva), "constantRva": hex(location),
                          "bits": hex(value)})
    return {"apiCalls": calls, "jitterConstants": constants, "fragments": fragments,
            "limits": "Static game-side paths; active branch/configuration and final texture encoding need additional tracing or runtime capture."}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--archive", required=True, type=Path)
    parser.add_argument("--extracted", required=True, type=Path)
    parser.add_argument("--sevenzip", default="C:/Program Files/7-Zip/7z.exe", type=Path)
    parser.add_argument("--output", required=True, type=Path)
    args = parser.parse_args()
    report = {"scope": "Static hashes, streamed archive members and unwind-bounded API loader/dispatch witnesses; no DLL execution",
              "sdkTag": "v3.0.2", "sdkCommit": SDK_COMMIT,
              "archive": identify(args.archive, ARCHIVE_PIN),
              "callers": {name: identify(args.extracted / name, pin) for name, pin in PE_PINS.items()},
              "runtimes": {name: runtime_member(args, name, pin) for name, pin in SDK_BLOBS.items()},
              "loaderWitnesses": loader_witnesses(args.extracted / "UpscalerBasePlugin/PDPerfPlugin.dll"),
              "srDispatch": sr_dispatch_witness(args.extracted / "UpscalerBasePlugin/PDPerfPlugin.dll"),
              "gameInputs": game_input_witnesses(args.extracted / "SKSE/Plugins/SkyrimUpscaler.dll"),
              "limits": "Loader and static dispatch witnesses do not establish runtime-selected frame parameters, buffer colour correctness or gameplay qualification."}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"PASS: {len(report['runtimes'])} official SDK runtime matches; {len(report['loaderWitnesses'])} verified loader witnesses; D3D12 SR dispatch chain; game-side input witnesses")


if __name__ == "__main__":
    main()
