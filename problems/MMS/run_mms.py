#!/usr/bin/env python3
"""
Run the MMS convergence test for DG + TVDRK2 on 3 successively refined meshes.

Usage:
    python run_mms.py                    # runs all 3 cases
    python run_mms.py --nx 100           # run single case N=100
    python run_mms.py --plot-only        # only plot from existing CSVs (skip runs)
"""

import subprocess
import sys
import os
import csv
import argparse
from pathlib import Path

# ── Configuration ──────────────────────────────────────────────────────────
APP = "cdf_update-opt"
INPUT_FILE = "mms_dg_tvd.i"
INPUT_FILE_CLEAN = "mms_dg_clean.i"
INPUT_FILE_HYBRID = "mms_dg_hybrid.i"
INPUT_FILE_LIMITED = "mms_dg_limited.i"
SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parent.parent

# Mesh sizes and corresponding time steps (dt ∝ h, CFL-safe for explicit DG)
CASES = [
    {"nx": 50,  "dt": 0.001,   "label": "N=50"},
    {"nx": 100, "dt": 0.0005,  "label": "N=100"},
    {"nx": 200, "dt": 0.00025, "label": "N=200"},
    {"nx": 400, "dt": 0.000125,"label": "N=400"},
]

# ── Helpers ────────────────────────────────────────────────────────────────

def find_app():
    """Locate the compiled MOOSE application binary."""
    candidates = [
        PROJECT_ROOT / f"{APP}",
        PROJECT_ROOT / f"{APP}.exe",
    ]
    for c in candidates:
        if c.exists():
            return str(c)
    # Try PATH fallback
    return APP


def run_case(nx, dt, label, app_path, input_file=None, csv_prefix="mms_out"):
    """Run a single mesh case.  Returns the (h, L2_error) tuple."""
    infile = input_file or INPUT_FILE
    csv_base = f"{csv_prefix}_n{nx}"
    cmd = [
        app_path,
        "-i", str(SCRIPT_DIR / infile),
        f"Mesh/gen/nx={nx}",
        f"Executioner/dt={dt}",
        f"Outputs/csv/file_base={csv_base}",
        "--no-trap-fpe",
    ]
    print(f"\n{'='*60}")
    print(f"  Running {label}: nx={nx}, dt={dt}")
    print(f"  Command: {' '.join(c for c in cmd if not c.startswith('--'))}")
    print(f"{'='*60}")

    result = subprocess.run(cmd, cwd=str(SCRIPT_DIR), capture_output=False)
    if result.returncode != 0:
        print(f"  ERROR: {label} failed with return code {result.returncode}")
        return None

    # Parse CSV to extract final L2 error
    csv_path = SCRIPT_DIR / f"{csv_prefix}_n{nx}.csv"
    return extract_error(csv_path, label, nx)


def extract_error(csv_path, label, nx):
    """Read the final L2 error from the CSV output.  h = 1/nx for pseudo-1D."""
    if not csv_path.exists():
        print(f"  WARNING: CSV not found at {csv_path}")
        return None

    with open(csv_path, "r") as f:
        reader = csv.DictReader(f)
        rows = list(reader)

    if not rows:
        print(f"  WARNING: CSV is empty: {csv_path}")
        return None

    last = rows[-1]
    try:
        l2 = float(last["l2_error"])
        t = float(last["time"])
    except (KeyError, ValueError) as e:
        print(f"  ERROR parsing CSV columns: {e}")
        print(f"  Available columns: {list(last.keys())}")
        return None

    h = 1.0 / nx                    # true dx for pseudo-1D mesh
    print(f"  {label}: t={t:.4f}, h=1/{nx}={h:.6f}, L2_error={l2:.6e}")
    return (h, l2)


# ── Main ───────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="MMS convergence test runner")
    parser.add_argument("--nx", type=int, default=0,
                        help="Run a single case with given nx (skip others)")
    parser.add_argument("--clean", action="store_true",
                        help="Use mms_dg_clean.i (MOOSE built-in kernels, no BCs)")
    parser.add_argument("--hybrid", action="store_true",
                        help="Use mms_dg_hybrid.i (built-in volume + custom DG kernel)")
    parser.add_argument("--limited", action="store_true",
                        help="Use mms_dg_limited.i (custom kernels + slope limiter)")
    parser.add_argument("--plot-only", action="store_true",
                        help="Skip runs, only collect and print errors from existing CSVs")
    args = parser.parse_args()

    if args.clean:
        input_file = INPUT_FILE_CLEAN
        csv_prefix = "mms_clean"
    elif args.hybrid:
        input_file = INPUT_FILE_HYBRID
        csv_prefix = "mms_hybrid"
    elif args.limited:
        input_file = INPUT_FILE_LIMITED
        csv_prefix = "mms_limited"
    else:
        input_file = INPUT_FILE
        csv_prefix = "mms_out"

    app_path = find_app()

    if args.nx > 0:
        case = next((c for c in CASES if c["nx"] == args.nx), None)
        if case is None:
            print(f"Error: nx={args.nx} not in pre-configured cases. Available: "
                  f"{[c['nx'] for c in CASES]}")
            sys.exit(1)
        if not args.plot_only:
            run_case(case["nx"], case["dt"], case["label"], app_path,
                     input_file=input_file, csv_prefix=csv_prefix)
        else:
            csv_path = SCRIPT_DIR / f"{csv_prefix}_n{args.nx}.csv"
            extract_error(csv_path, case["label"], case["nx"])
        return

    results = []
    for case in CASES:
        if args.plot_only:
            csv_path = SCRIPT_DIR / f"{csv_prefix}_n{case['nx']}.csv"
            r = extract_error(csv_path, case["label"], case["nx"])
        else:
            r = run_case(case["nx"], case["dt"], case["label"], app_path,
                         input_file=input_file, csv_prefix=csv_prefix)
        if r:
            results.append({"nx": case["nx"], "h": r[0], "l2_error": r[1], "label": case["label"]})

    # Print summary
    print(f"\n{'='*60}")
    print("  Convergence Summary")
    print(f"{'='*60}")
    print(f"  {'Case':>8s}  {'h':>10s}  {'L2 Error':>14s}")
    print(f"  {'-'*36}")
    for r in results:
        print(f"  {r['label']:>8s}  {r['h']:10.6f}  {r['l2_error']:14.6e}")

    if len(results) >= 2:
        import math
        for i in range(len(results) - 1):
            h1, e1 = results[i]["h"], results[i]["l2_error"]
            h2, e2 = results[i+1]["h"], results[i+1]["l2_error"]
            slope = math.log(e2 / e1) / math.log(h2 / h1)
            label = f"{results[i]['label']} → {results[i+1]['label']}"
            print(f"  {label:>16s}  slope = {slope:.3f}")

    print(f"{'='*60}")


if __name__ == "__main__":
    main()
