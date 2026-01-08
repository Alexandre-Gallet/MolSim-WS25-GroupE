#!/bin/bash
#SBATCH -J molsim_rt_large
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

# Load runtime environment (not inherited from login shell)
module load gcc/14.2.0

# Run simulation
echo "Job started on node: $(hostname)"
echo "Current directory: $(pwd)"
echo "Binary:"
ls -l $HOME/MolSim-WS25-GroupE/build/MolSim
echo "Input file:"
ls -l $HOME/MolSim-WS25-GroupE/input/eingabeRayleighTaylorBigBenchmarking.yml

echo "Running MolSim now..."

$HOME/MolSim-WS25-GroupE/build/MolSim \
$HOME/MolSim-WS25-GroupE/input/eingabeRayleighTaylorBigBenchmarking.yml

echo "MolSim finished with exit code $?"


