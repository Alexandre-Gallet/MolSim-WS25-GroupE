#!/bin/bash
#SBATCH -J molsim_bench_omp
#SBATCH -D /home/$USER/MolSim-WS25-GroupE
#SBATCH -o %x.%j.%N.out
#SBATCH -e %x.%j.%N.err
#SBATCH --mail-user=ge63duc@tum.de
#SBATCH --mail-type=END,FAIL
#SBATCH --get-user-env
#SBATCH --export=NONE

# CoolMUC-4: shared-memory/OpenMP job on a single node -> use cm4_tiny
#SBATCH --clusters=cm4
#SBATCH --partition=cm4_tiny

# One process, many threads
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32

# adjust as needed
#SBATCH --time=01:00:00

# Load runtime environment (modules are not inherited in batch jobs)
module load slurm_setup
module load gcc/14.2.0

# OpenMP thread count = cores reserved by Slurm for this task
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
export OMP_PLACES=cores
export OMP_PROC_BIND=close

echo "=== Job info ==="
echo "Job started on node: $(hostname)"
echo "Workdir: $(pwd)"
echo "SLURM_JOB_ID: $SLURM_JOB_ID"
echo "Threads (cpus-per-task): $SLURM_CPUS_PER_TASK"
echo "OMP_NUM_THREADS: $OMP_NUM_THREADS"
echo

echo "=== Binaries sanity check ==="
ls -l ./build/src/MolSimBenchmark || true
ls -l ./scripts/run_benchmark.sh || true
echo

# Choose ONE of the following execution variants:

echo "=== Running benchmark (preferred: your timestamped script) ==="
# srun is the recommended way to launch inside a batch allocation on CoolMUC
srun --exact -n 1 -c "$SLURM_CPUS_PER_TASK" \
  bash ./scripts/run_benchmark.sh ./input/eingabe.yml

# --- OR (fallback) ---
# echo "=== Running benchmark (direct executable fallback) ==="
# srun --exact -n 1 -c "$SLURM_CPUS_PER_TASK" \
#   ./build/src/MolSimBenchmark ./input/eingabe.yml

echo
echo "MolSim finished with exit code $?"
