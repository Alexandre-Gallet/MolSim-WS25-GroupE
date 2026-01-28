#!/usr/bin/env bash
# Run MolSimGprof with a given YAML input and generate a timestamped,
# git-annotated, host-annotated gprof report.
#
# The script intentionally:
#  - enforces exactly one input file
#  - records time, git commit, host, input hash
#  - deletes gmon.out after generating the report
#  - produces a single self-contained .txt file

set -euo pipefail

# Configuration
# Build directory can be overridden via environment variable:
#   BUILD_DIR=build-gprof ./run_gprof.sh input.yml
BUILD_DIR="${BUILD_DIR:-build}"

# Instrumented executable
EXE="./${BUILD_DIR}/src/MolSimGprof"

# Where profiling reports are stored
OUT_DIR="gprof_txt_output"

# Input handling
# Exactly one argument: path to YAML configuration
if [[ $# -ne 1 ]]; then
  echo "Usage: $0 path/to/config.yml"
  exit 1
fi

CFG="$1"

# Sanity checks
# Ensure executable exists and is runnable
if [[ ! -x "${EXE}" ]]; then
  echo "Error: ${EXE} not found or not executable"
  exit 1
fi

# Ensure YAML file exists
if [[ ! -f "${CFG}" ]]; then
  echo "Error: config file not found: ${CFG}"
  exit 1
fi

# Create output directory if needed
mkdir -p "${OUT_DIR}"

# Metadata collection
# Timestamp (sortable and filename-safe)
TS="$(date +%Y%m%d_%H%M%S)"

# Git commit hash (or fallback if not called from inside a git repo)
if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  GIT_SHA="$(git rev-parse --short HEAD)"
else
  GIT_SHA="nogit"
fi

# Short hostname (useful for clusters)
HOST="$(hostname -s 2>/dev/null || echo unknown)"

# Input file metadata
CFG_BASENAME="$(basename "${CFG}")"
CFG_HASH="$(sha1sum "${CFG}" | cut -d' ' -f1)"

# Output filename
BASE="gprof_${TS}_${GIT_SHA}_${HOST}_${CFG_BASENAME}"
REPORT="${OUT_DIR}/${BASE}.txt"

# Run profiling
echo "[gprof] Running ${EXE} ${CFG}"
"${EXE}" "${CFG}"

# Ensure profiling data was generated
if [[ ! -f gmon.out ]]; then
  echo "Error: gmon.out not generated"
  exit 1
fi

# Generate report
# Write metadata header + full gprof output
{
  echo "=== MolSim gprof report ==="
  echo "Timestamp     : ${TS}"
  echo "Git commit    : ${GIT_SHA}"
  echo "Host          : ${HOST}"
  echo "Input file    : ${CFG}"
  echo "Input SHA1    : ${CFG_HASH}"
  echo
  gprof "${EXE}" gmon.out
} > "${REPORT}"

# Cleanup
# Remove binary profiling output to avoid clutter
rm -f gmon.out

echo "[gprof] Report written to ${REPORT}"
