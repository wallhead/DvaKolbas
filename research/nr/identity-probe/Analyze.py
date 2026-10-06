"""Validate experiment identities, compare every source hash and summarize tone.

This measures a bounded synthetic fixture. It has no gameplay acceptance oracle.
"""
import argparse
import csv
import hashlib
import json
import re
import statistics
from pathlib import Path


def sha(path):
    with path.open("rb") as handle:
        return hashlib.file_digest(handle, "sha256").hexdigest()


def load_frames(path):
    with path.open(newline="") as handle:
        rows = list(csv.DictReader(handle))
    assert rows
    for index, row in enumerate(rows, 1):
        assert int(row["source"]) == index
        assert re.fullmatch(r"[0-9a-f]{64}", row["outputSha256"])
    return rows


def identity_summary(path, variant, passes):
    parsed = [dict(re.findall(r"(\w+)=([\w]+)", line))
              for line in path.read_text().splitlines() if line.startswith("NR_ID ")]
    assert len(parsed) == 12 * passes
    result = []
    for number in range(1, passes + 1):
        rows = [r for r in parsed if int(r["pass"]) == number]
        assert [int(r["source"]) for r in rows] == list(range(1, 13))
        counts = {key: len({r[key] for r in rows})
                  for key in ["color", "depth", "motion", "output", "params", "creation"]}
        fixed_io = variant in {"fixed-resources", "fixed-both", "fixed-io-rotating-params"}
        assert all(counts[k] == (1 if fixed_io else 3)
                   for k in ["color", "depth", "motion", "output"])
        fixed_params = variant in {"fixed-parameters", "fixed-resources", "fixed-both"}
        assert counts["params"] == (1 if fixed_params else 3)
        assert counts["creation"] == 1
        creation_used = variant in {"fixed-parameters", "fixed-both"}
        assert all((r["params"] == r["creation"]) == creation_used for r in rows)
        result.append({"pass": number, "unique": counts, "creationObjectUsed": creation_used,
                       "maxPendingTicketsInFirst12": max(int(r["pending"]) for r in rows)
                       if all("pending" in r for r in rows) else None})
    return result


def describe(rows, scene):
    assert sum(int(r["reset"]) for r in rows) == 1
    if scene == "context":
        assert len(rows) == 160
        phases = [[statistics.mean(float(r[c]) for r in rows[start:start + 20])
                   for c in ["meanR", "meanG", "meanB"]]
                  for start in [20, 60, 100, 140]]
        sources = [[statistics.mean(float(r[c]) for r in rows[start:start + 20])
                    for c in ["sourceR", "sourceG", "sourceB"]]
                   for start in [20, 60, 100, 140]]
        assert max(max(p[c] for p in sources) - min(p[c] for p in sources)
                   for c in range(3)) < 1e-6
        return {"settledPhaseMeanRgb": phases,
                "maxSettledChannelDriftCodeValues": max(max(p[c] for p in phases) - min(p[c] for p in phases)
                                                       for c in range(3)),
                "inputSurfaceDrift": 0}
    assert len(rows) == 120
    return {"settledWarpedRgbDifference": {
        name: statistics.mean(float(r["warpedMeanRgbDifference"]) for r in rows[start:start + 20])
        for name, start in [("stationary", 20), ("pan", 50), ("stationaryAfterPan", 100)]}}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--serial", type=Path, required=True)
    parser.add_argument("--deferred", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    results, data = [], {}
    for capture, root in [("serial", args.serial), ("deferred", args.deferred)]:
        for receipt in [root / "runs.json", root / "runs-io-isolation.json"]:
            manifest = json.loads(receipt.read_text(encoding="utf-8-sig"))
            for run in manifest["runs"]:
                directory = Path(run["directory"])
                frames = load_frames(directory / "frames.csv")
                assert len(frames) == run["sources"]
                assert all(r["nrEvaluated"] == "1" for r in frames)
                assert sha(directory / "frames.csv") == run["csvSha256"]
                assert sha(root / f"NrIdentityProbe-{run['variant']}.exe") == run["executableSha256"]
                identities = identity_summary(directory / "run.log", run["variant"], run["passes"])
                key = (capture, run["variant"], run["case"])
                data[key] = frames
                results.append({**run, "capture": capture, "identity": identities,
                                "measurements": describe(frames, run["scene"]),
                                "logSha256": sha(directory / "run.log")})
        for scene in ["pan", "context"]:
            off = load_frames(root / f"off-{scene}" / "frames.csv")
            assert len(off) == (120 if scene == "pan" else 160)
            assert all(r["nrEvaluated"] == "0" and float(r["meanRgbCorrection"]) == 0 for r in off)
    comparisons = []
    for (capture, variant, case), frames in data.items():
        baseline = data[("serial", "rotating", case)]
        assert len(frames) == len(baseline)
        mismatches = [int(b["source"]) for a, b in zip(baseline, frames)
                      if a["outputSha256"] != b["outputSha256"]]
        comparisons.append({"capture": capture, "variant": variant, "case": case,
                            "comparedSources": len(frames), "hashMismatches": mismatches})
    output = {"schema": 1, "date": "2026-10-06", "evaluatedSources": sum(r["sources"] for r in results),
              "offSources": 560, "runs": results, "comparisonsToSerialRotating": comparisons,
              "scope": "Native 640x360 SDR; same runtime/driver, Style0 Tone1, fixed depth, jitter0, no SR/FG/Skyrim",
              "interpretation": "Identity and pending-queue comparisons are synthetic evidence; game drift remains unresolved"}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2) + "\n")
    print(json.dumps({"runs": len(results), "evaluatedSources": output["evaluatedSources"],
                      "mismatchedSources": sum(len(c["hashMismatches"]) for c in comparisons)}, indent=2))


if __name__ == "__main__":
    main()
