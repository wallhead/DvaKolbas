"""Check settled fixed-surface RGB across the four context-probe phases."""
import argparse
import csv
import json
import statistics
import sys

parser = argparse.ArgumentParser()
parser.add_argument("csv")
parser.add_argument("--max-code-drift", type=float, default=1.0)
args = parser.parse_args()
with open(args.csv, newline="", encoding="utf-8") as stream:
    rows = list(csv.DictReader(stream))
if len(rows) != 160 or [int(r["source"]) for r in rows] != list(range(1, 161)):
    sys.exit("FAIL incomplete context capture")
means = []
source = []
for phase in range(4):
    settled = rows[phase * 40 + 20:(phase + 1) * 40]
    means.append([statistics.mean(float(r[k]) for r in settled) for k in ("meanR", "meanG", "meanB")])
    source.append([statistics.mean(float(r[k]) for r in settled) for k in ("sourceR", "sourceG", "sourceB")])
if source != [source[0]] * 4:
    sys.exit("FAIL surface source changed; this is not the fixed-surface regression")
drift = [max(p[c] for p in means) - min(p[c] for p in means) for c in range(3)]
passed = max(drift) <= args.max_code_drift
print(json.dumps({"passed": passed, "phaseMeans": means, "rgbCodeDrift": drift,
                  "threshold": args.max_code_drift}, indent=2))
sys.exit(0 if passed else 1)
