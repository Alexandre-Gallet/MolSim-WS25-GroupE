#!/bin/bash
#SBATCH -J molsim_gprof_large
#SBATCH -D ./
#SBATCH -o %x.%j.out
#SBATCH -e %x.%j.err
#SBATCH --mail-user=ge63duc@tum.de
#SBATCH --mail-type=END,FAIL
#SBATCH --get-user-env
#SBATCH --export=NONE
#SBATCH --clusters=serial
#SBATCH --partition=serial_std
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=02:00:00

module load gcc/14.2.0

BIN=$HOME/MolSim-WS25-GroupE/build-gprof/MolSim
INP=$HOME/MolSim-WS25-GroupE/input/eingabeRayleighTaylorBigBenchmarking.yml

echo "Node: $(hostname)"
echo "Binary: $BIN"
echo "Input:  $INP"
ls -l "$BIN" "$INP"

# Run (creates gmon.out in the current working directory)
"$BIN" "$INP"
rc=$?

true

echo "MolSim exit code: $rc"

# Save profiling file with unique name
mv -f gmon.out "gmon.${SLURM_JOB_ID}.out"
echo "Saved profiling data to gmon.${SLURM_JOB_ID}.out"

exit $rc

