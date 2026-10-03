"""Static, hash-gated AIO19 performance witnesses; never loads/executes a DLL.

Requires pefile and capstone. Extract the four named archive members beforehand.
The short instruction windows are witnesses, not inferred function boundaries.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

import capstone
import pefile


ARTIFACTS = {
    "SKSE/Plugins/SkyrimUpscaler.dll": (15975424, "ac699b5883d4deeafdfff81ab041c3f191b1970e68c3408ffdb18068a6f5482a"),
    "SKSE/Plugins/SkyrimUpscaler.ini": (15300, "ad0b46779903a9fec921a3fedb4f03c84eb393fd646ba889e5875608a6979967"),
    "UpscalerBasePlugin/PDPerfPlugin.dll": (20332032, "ff6c58391501006474e36afaac2b5a5cfceda47850abdbbcc7f2fe5521004435"),
    "UpscalerBasePlugin/nvngx_dlssnr.dll": (165840496, "8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206"),
}
ARCHIVE = (204042400, "49e7f7dabf426937915d1aeed664fc40a7cc7d89f42092a69c205b22c4687439")
EXPORTS = {
    "InitDLSSNR": 0x1177E0, "EvaluateDLSSNR": 0x117810,
    "EvaluateDLSSNRChain": 0x1178D0, "QueryDLSSNRChainStatus": 0x117910,
    "QueryDLSSNRMaskCapabilities": 0x1179F0, "ReleaseDLSSNR": 0x117A50,
}
SOURCE_BLOBS = {
    "src/NeuralRendering/BeforeUpscale.cpp": "432da9b6be167278f2ba2780de2e00384a16b175",
    "src/NeuralRendering/PreparedBeforeUpscale.cpp": "013dfc146b121ca0664d84fdc10eafeea79925a6",
    "src/Graphics/D3D11D3D12Interop.cpp": "5cb677fe4788ef3e7aea5eb450bcea7aaf326764",
    "src/Graphics/D3D11D3D12Interop.h": "57be88c6e1b9075f0db4077baaf2497b7be9820d",
    "src/NeuralRendering/Stage.cpp": "8472af23269fc9b44ab8637168fbf28e43bd69ea",
    "src/NeuralRendering/ImagePacket.cpp": "b8262fc28fe7cc0346857a345afef1592e0cd38c",
    "src/NeuralRendering/BeforeHost.cpp": "414b89e8701befa70d6ae171e7c2ad1db52282d2",
    "tools/nr/runtime-pin.json": "765f9e36fc2c54b1448ac4971c0cebd8f26aa7a9",
}
# RVA, expected first-instruction bytes, interpretation; indirect COM method
# names are inferred from the receiver and documented interface layout.
WITNESSES = [
    (0x1178D5, "813998050000", "Chain ABI size 0x598"),
    (0x1178DD, "83790401", "Chain ABI version 1"),
    (0x10F955, "448ba974010000", "Pass count at +0x174"),
    (0x10F960, "83f809", "Unsigned count-minus-one bound: count 1..10"),
    (0xA607E, "488d967c010000", "Pass table at +0x17c"),
    (0xA6085, "486bc068", "Pass stride 0x68"),
    (0x1110B7, "e8144ff9ff", "Local chain entry calls 0xa5fd0"),
    (0xA627C, "e85f000000", "Chain wrapper calls 0xa62e0"),
    (0xA6557, "e804eeffff", "Conditional local evaluation calls 0xa5360"),
    (0xA5448, "ff9098040000", "D3D11 producer Signal (+0x498)"),
    (0xA5454, "ff9078030000", "D3D11 producer Flush (+0x378)"),
    (0xA5479, "ff5078", "D3D12 queue Wait (+0x78)"),
    (0xA547C, "eb0c", "Normal fence branch skips fallback Flush at 0xa5484"),
    (0xA548A, "e80102fdff", "Begin persistent local command slot (0x75690)"),
    (0x756CF, "e8dc040000", "Begin calls slot selector (0x75bb0)"),
    (0x75C29, "ff5040", "Slot completion check: fence GetCompletedValue"),
    (0x75C33, "83fe08", "Selector scans eight slots"),
    (0x75C72, "e8c9a8ffff", "Saturated selector calls wait helper (0x70540)"),
    (0x705F7, "ff5048", "Fence SetEventOnCompletion"),
    (0x705FA, "baffffffff", "OS wait timeout INFINITE"),
    (0x70602, "ff15", "Imported WaitForSingleObject; import verified separately"),
    (0x702F3, "ff9028010000", "Device GetDeviceRemovedReason (+0x128)"),
    (0x7579A, "ff5048", "List Close; subsequent HRESULT test"),
    (0x757E2, "ff5050", "ExecuteCommandLists"),
    (0x75809, "ff5070", "Queue completion Signal"),
    (0x75813, "488984fb980c0000", "Store completion target indexed by selected slot"),
    (0xA57E0, "e86bfffcff", "Submit local command slot (0x75750)"),
    (0xA5800, "4c8b89a0040000", "D3D11 consumer Wait method (+0x4a0)"),
    (0xA580D, "41ffd1", "Queue D3D11 consumer wait via r9"),
    (0xA5852, "4c8b9070010000", "D3D11 regional output copy method (+0x170)"),
    (0xA5899, "ff9078010000", "Alternative full output CopyResource (+0x178)"),
    (0x11794A, "488d88d0310000", "Status lock at backend +0x31d0"),
]


def identity(path):
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return {"bytes": path.stat().st_size, "sha256": digest.hexdigest()}


def pinned(path, expected):
    actual = identity(path)
    if (actual["bytes"], actual["sha256"]) != expected:
        raise ValueError(f"Artifact identity mismatch: {path.name}")
    return actual


def inspect(args):
    root = Path(__file__).resolve().parents[2]
    git = lambda *parts: subprocess.check_output(
        ["git", *parts], cwd=root, text=True).strip()
    source_revision = git("rev-parse", args.revision + "^{commit}")
    baseline = git("rev-parse", "de4662a^{commit}")
    count = int(git("rev-list", "--count", f"{baseline}..{source_revision}"))
    if count != 4:
        raise ValueError("Source checkpoint commit relation mismatch")
    blobs = {}
    for path, expected in SOURCE_BLOBS.items():
        actual = git("rev-parse", f"{source_revision}:{path}")
        if actual != expected:
            raise ValueError(f"Source checkpoint mismatch: {path}")
        if git("rev-parse", f"{baseline}:{path}") != actual:
            raise ValueError(f"Core blob changed since checkpoint baseline: {path}")
        blobs[path] = actual
    artifacts = {name: pinned(args.extracted / name, expected)
                 for name, expected in ARTIFACTS.items()}
    archive = pinned(args.archive, ARCHIVE)
    pe = pefile.PE(str(args.extracted / "UpscalerBasePlugin/PDPerfPlugin.dll"))
    base = pe.OPTIONAL_HEADER.ImageBase
    exports = {e.name.decode(): e.address for e in pe.DIRECTORY_ENTRY_EXPORT.symbols if e.name}
    for name, expected in EXPORTS.items():
        if exports.get(name) != expected:
            raise ValueError(f"Export mismatch: {name}")
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
    decoder.detail = True
    witnesses = []
    for rva, byte_prefix, meaning in WITNESSES:
        instructions = list(decoder.disasm(pe.get_data(rva, 32), base + rva, count=3))
        if not instructions or not instructions[0].bytes.hex().startswith(byte_prefix):
            raise ValueError(f"Instruction mismatch at {rva:#x}")
        witnesses.append({"rva": hex(rva), "interpretation": meaning,
                          "instructions": [{"rva": hex(i.address - base), "bytes": i.bytes.hex(),
                                            "text": f"{i.mnemonic} {i.op_str}"} for i in instructions]})
    wait = next(decoder.disasm(pe.get_data(0x70602, 6), base + 0x70602))
    operand = wait.operands[0]
    iat_address = wait.address + wait.size + operand.mem.disp
    imports = {i.address: (m.dll.decode(), i.name.decode() if i.name else str(i.ordinal))
               for m in pe.DIRECTORY_ENTRY_IMPORT for i in m.imports}
    if imports.get(iat_address) != ("KERNEL32.dll", "WaitForSingleObject"):
        raise ValueError("OS wait import mismatch")
    report = {"schema": 1, "analysisScope": "Static PE/Capstone and source identity; no DLL execution",
              "sourceRevision": source_revision, "archive": archive, "artifacts": artifacts,
              "checkpointRelation": {"base": baseline, "commitsAhead": count,
                                     "coreBlobsUnchanged": True},
              "dependencies": {"capstone": capstone.__version__, "pefile": pefile.__version__},
              "checkpointBlobs": blobs, "exports": {n: hex(exports[n]) for n in EXPORTS},
              "witnesses": witnesses, "osWaitImport": list(imports[iat_address]),
              "notEstablished": ["Measured AIO19 vs Dva source-frame cost",
                                 "How often either host actually blocks or saturates",
                                 "Live NR input extent/color domain and guide provenance",
                                 "Same-host model runtime A/B compatibility/cost",
                                 "Transitive allocation/waits inside vendor or proxy calls"]}
    if args.documents:
        report["suppliedDocuments"] = {p.name: identity(p) for p in args.documents}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"PASS: {len(artifacts)} artifacts, {len(blobs)} source blobs, "
          f"{len(EXPORTS)} exports, {len(witnesses)} instruction witnesses; static only")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--archive", type=Path, required=True)
    parser.add_argument("--extracted", type=Path, required=True)
    parser.add_argument("--revision", default="27ad903")
    parser.add_argument("--documents", type=Path, nargs="*")
    parser.add_argument("--output", type=Path, required=True)
    try:
        inspect(parser.parse_args())
    except (ValueError, OSError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"FAIL: {error}\n")
