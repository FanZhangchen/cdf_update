#!/bin/bash
# ============================================================================
# Batch run: BLP L400 LimiterStudy — three cases, 12 cores each
#
# Usage:
#   chmod +x run_limiter_study.sh
#   ./run_limiter_study.sh              # sequential, each with 12 cores
#   ./run_limiter_study.sh --slurm      # submit 3 independent SLURM jobs
# ============================================================================

set -e

APP="/home/zhangchen/projects/cdf_update/cdf_update-opt"
STUDY_DIR="/home/zhangchen/projects/cdf_update/problems/BLP_L400/LimiterStudy"
NP=12
THREADS=1

cd "$STUDY_DIR"

# ── SLURM mode ─────────────────────────────────────────────────────────────
if [ "${1:-}" = "--slurm" ]; then
    for CASE in original dg dg_limited; do
        JOBNAME="BLP_${CASE}"
        sbatch <<-EOF
#!/bin/bash
#SBATCH --job-name=${JOBNAME}
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=${NP}
#SBATCH --cpus-per-task=1
#SBATCH --time=48:00:00
#SBATCH --output=${STUDY_DIR}/${JOBNAME}_%j.out
#SBATCH --error=${STUDY_DIR}/${JOBNAME}_%j.err

module purge
module load gcc openmpi

mpiexec -n ${NP} ${APP} -i BLP_L400_${CASE}.i --n-threads=${THREADS}
EOF
        echo "Submitted: ${JOBNAME}"
    done
    exit 0
fi

# ── Sequential nohup mode ──────────────────────────────────────────────────
LOGFILE="${STUDY_DIR}/run_limiter_study.log"
echo "=== LimiterStudy batch started at $(date) ===" | tee -a "$LOGFILE"

for CASE in original dg dg_limited; do
    echo "" | tee -a "$LOGFILE"
    echo "--- ${CASE}  (start: $(date)) ---" | tee -a "$LOGFILE"
    mpiexec -n ${NP} ${APP} -i BLP_L400_${CASE}.i --n-threads=${THREADS} \
        2>&1 | tee -a "$LOGFILE"
    echo "--- ${CASE}  (done:  $(date)) ---" | tee -a "$LOGFILE"
done

echo "" | tee -a "$LOGFILE"
echo "=== All done at $(date) ===" | tee -a "$LOGFILE"
