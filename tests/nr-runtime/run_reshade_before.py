"""Run the NR fence and complete Before-host regressions with supplied ReShade.

Only an isolated test directory is modified. Both executables refuse GPU work
while Skyrim is running; neither this runner nor its probes launch Skyrim.
"""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

parser = argparse.ArgumentParser()
for name in ("packet_exe", "host_exe", "runtime", "runtime_root", "driver_core", "output"):
    parser.add_argument("--" + name.replace("_", "-"), type=Path, required=True)
parser.add_argument("--deferred-exe", type=Path)
args = parser.parse_args()
args.output.mkdir(parents=True, exist_ok=True)
result_file = args.output / "results.json"
result_file.unlink(missing_ok=True)
records = []
with tempfile.TemporaryDirectory(prefix="nr-reshade-", dir=args.output.resolve()) as temporary:
    root = Path(temporary)
    for exe in (args.packet_exe, args.host_exe, *([args.deferred_exe] if args.deferred_exe else [])):
        shutil.copy2(exe, root / exe.name)
    shutil.copy2(args.runtime, root / "dxgi.dll")
    shutil.copy2(Path(__file__).parent.parent / "fixtures" / "reshade" / "ReShade.ini", root)
    cases = (
        ("packet", args.packet_exe, ["--require-wrapped"]),
        ("host", args.host_exe, [str(args.runtime_root.resolve()), str(args.driver_core.resolve()),
                                str((args.output / "host.json").resolve()), "--require-wrapped"]),
    )
    if args.deferred_exe:
        cases += tuple(("deferred-" + mode, args.deferred_exe,
                        [str((args.runtime_root / "NR" / "rtx40" / "nvngx_dlssnr.dll").resolve()),
                         str(args.driver_core.resolve()), mode, "--require-wrapped"])
                       for mode in ("pressure", "retire"))
    for name, exe, extra in cases:
        result = subprocess.run([str(root / exe.name), *extra], cwd=root,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=180)
        (args.output / (name + ".log")).write_bytes(result.stdout)
        print(result.stdout.decode("utf-8", errors="replace"), flush=True)
        if result.returncode:
            raise SystemExit(result.returncode)
        records.append({"case": name, "result": "PASS",
                        "exeSha256": hashlib.sha256(exe.read_bytes()).hexdigest()})
result_file.write_text(json.dumps({
    "runtimeSha256": hashlib.sha256(args.runtime.read_bytes()).hexdigest(),
    "skyrimTested": False, "wrappedDeviceRequired": True, "cases": records,
}, indent=2) + "\n")
