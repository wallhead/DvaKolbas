"""Read-only NR catalog/provider inventory. Never loads a DLL or installs hooks.

Requires pefile for the provider export/import listing. Output is bounded text
evidence; PE exports alone do not establish a writable post-generation output.
"""
import argparse
import hashlib
import json
import re
from pathlib import Path
import pefile


def sha256(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def provider(path):
    pe = pefile.PE(str(path), fast_load=True)
    pe.parse_data_directories(directories=[
        pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"],
        pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"],
    ])
    export_directory = getattr(pe, "DIRECTORY_ENTRY_EXPORT", None)
    exports = [{"name": e.name.decode("ascii", "replace"), "rva": hex(e.address)}
               for e in (export_directory.symbols if export_directory else ()) if e.name]
    imports = [{"module": d.dll.decode("ascii", "replace"),
                "symbols": [s.name.decode("ascii", "replace") for s in d.imports if s.name]}
               for d in getattr(pe, "DIRECTORY_ENTRY_IMPORT", ())]
    return {"path": str(path.resolve()), "bytes": path.stat().st_size,
            "sha256": sha256(path), "machine": hex(pe.FILE_HEADER.Machine),
            "sizeOfImage": pe.OPTIONAL_HEADER.SizeOfImage,
            "exports": exports, "imports": imports,
            "interpretation": "static inventory; no output/guide ownership observed"}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--driver-inf", type=Path, required=True)
    p.add_argument("--catalog-source", type=Path, required=True)
    p.add_argument("--provider", type=Path, action="append", default=[])
    p.add_argument("--output", type=Path, required=True)
    a = p.parse_args()
    source = a.catalog_source.read_text(encoding="utf-8")
    inf = a.driver_inf.read_text(encoding="utf-16" if a.driver_inf.read_bytes()[:2] == b"\xff\xfe" else "utf-8-sig")
    names = dict(re.findall(r'NVIDIA_DEV\.([0-9A-Fa-f]{4})\s*=\s*"([^"]+)"', inf))
    names = {k.lower(): v for k, v in names.items()}
    rows = []
    for series, ids in re.findall(r"constexpr std::array rtx(\d+)\{([^}]+)\}", source):
        for device in re.findall(r"0x([0-9a-f]{4})u", ids):
            name = names.get(device)
            expected = rf"^NVIDIA GeForce RTX {series}\d{{2}}(?: Ti| SUPER| Ti SUPER| OEM| D| D v2)?$"
            rows.append({"deviceId": f"10de:{device}", "catalogFamily": f"RTX {series}",
                         "infName": name, "familyNameMatches": bool(name and re.fullmatch(expected, name))})
    result = {"schema": 1, "driverInf": {"path": str(a.driver_inf.resolve()),
              "sha256": sha256(a.driver_inf)}, "catalogSourceSha256": sha256(a.catalog_source),
              "desktopIds": rows, "allIdsVerified": bool(rows) and all(r["familyNameMatches"] for r in rows),
              "providers": [provider(path) for path in a.provider],
              "hardwareQualification": "NOT RUN by this offline tool"}
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(f"catalog IDs={len(rows)} verified={result['allIdsVerified']}; modules={len(result['providers'])}")
    if not result["allIdsVerified"]:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
