#!/usr/bin/env python3
"""
Batch run BLP L400 LimiterStudy: original, DG, and DG+limiter.

Usage:
    python run_study.py              # sequential, 12 cores each
    python run_study.py -n 16        # 16 cores
    python run_study.py --slurm      # submit 3 independent SLURM jobs
"""

import os, sys, argparse
from datetime import datetime

# ── Config ──────────────────────────────────────────────────────────────────
APP   = "cdf_update-opt"
CASES = ["BLP_L400_original", "BLP_L400_dg", "BLP_L400_dg_limited"]
NP    = 12
THREADS = 1
STUDY_DIR = os.path.dirname(os.path.abspath(__file__))

# Locate the app binary (same pattern as existing main.py)
PROJECT_ROOT = os.path.abspath(os.path.join(STUDY_DIR, "..", "..", ".."))
APP_PATH = os.path.join(PROJECT_ROOT, APP)


def run_case(case_name, np, logfile):
    """Run a single case and return (success, elapsed)."""
    infile = f"{case_name}.i"
    t0 = datetime.now()
    tag = f"[{t0.strftime('%H:%M:%S')}] {case_name}"

    cmd = (f"mpiexec -n {np} {APP_PATH} -i {infile} --n-threads={THREADS} "
           f">> {logfile} 2>&1")

    print(f"{tag}  starting  (np={np}, threads={THREADS})")
    ret = os.system(cmd)
    elapsed = (datetime.now() - t0).total_seconds() / 60.0

    if ret == 0:
        print(f"{tag}  done  ({elapsed:.1f} min)")
    else:
        print(f"{tag}  FAILED  (rc={ret}, {elapsed:.1f} min)")

    return ret == 0, elapsed


def slurm_submit(case_name):
    """Submit one SLURM job and return job ID."""
    script = f"""#!/bin/bash
#SBATCH --job-name={case_name}
#SBATCH --nodes=1
#SBATCH --ntasks-per-node={NP}
#SBATCH --cpus-per-task=1
#SBATCH --time=48:00:00
#SBATCH --output={STUDY_DIR}/{case_name}_%j.out
#SBATCH --error={STUDY_DIR}/{case_name}_%j.err

module purge
module load gcc openmpi
mpiexec -n {NP} {APP_PATH} -i {case_name}.i --n-threads={THREADS}
"""
    import subprocess
    result = subprocess.run(["sbatch"], input=script, capture_output=True, text=True)
    print(result.stdout.strip())
    return result.returncode == 0


# ── Main ────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="BLP L400 LimiterStudy runner")
    parser.add_argument("-n", type=int, default=NP, help=f"Number of MPI processes (default: {NP})")
    parser.add_argument("--slurm", action="store_true", help="Submit SLURM jobs instead")
    args = parser.parse_args()

    if args.slurm:
        for case in CASES:
            slurm_submit(case)
        sys.exit(0)

    # Sequential mode
    logfile = os.path.join(STUDY_DIR, "run_study.log")
    with open(logfile, "w") as f:
        f.write(f"=== LimiterStudy batch  (start: {datetime.now()}) ===\n\n")

    passed = 0
    for case in CASES:
        ok, t = run_case(case, args.n, logfile)
        if ok:
            passed += 1

    with open(logfile, "a") as f:
        f.write(f"\n=== Done: {passed}/{len(CASES)} passed  ({datetime.now()}) ===\n")

    print(f"\n{paused}/{len(CASES)} cases passed.  Log: {logfile}")
