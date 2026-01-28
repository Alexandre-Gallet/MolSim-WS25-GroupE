#!/usr/bin/env bash
set -euo pipefail

# Configuration
BUILD_DIR="${BUILD_DIR:-build}"
EXE="./${BUILD_DIR}/src/MolSimBenchmark"
TXT_OUT_DIR="benchmark_txt_output"
CSV_OUT_DIR="benchmark_csv_output"

# Input handling
if [[ $# -ne 1 ]]; then
  echo "Usage: $0 path/to/config.yml"
  exit 1
fi

CFG="$1"

if [[ ! -x "${EXE}" ]]; then
  echo "Error: ${EXE} not found or not executable"
  exit 1
fi

if [[ ! -f "${CFG}" ]]; then
  echo "Error: config file not found: ${CFG}"
  exit 1
fi

mkdir -p "${TXT_OUT_DIR}" "${CSV_OUT_DIR}"

# Metadata
TS="$(date +%Y%m%d_%H%M%S)"

if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  GIT_SHA="$(git rev-parse --short HEAD)"
else
  GIT_SHA="nogit"
fi

HOST="$(hostname -s 2>/dev/null || echo unknown)"

CFG_BASENAME="$(basename "${CFG}")"
CFG_HASH="$(sha1sum "${CFG}" | cut -d' ' -f1)"

BASE="benchmark_${TS}_${GIT_SHA}_${HOST}_${CFG_BASENAME}"

TXT_REPORT="${TXT_OUT_DIR}/${BASE}.txt"
CSV_REPORT="${CSV_OUT_DIR}/${BASE}.csv"

# Run benchmark
echo "[benchmark] Running ${EXE} ${CFG}"
OUT="$("${EXE}" "${CFG}")"

# Extract runtime
RUNTIME="$(echo "${OUT}" | awk -F': ' '/^Runtime \[s\]:/ {print $2}')"

if [[ -z "${RUNTIME}" ]]; then
  echo "Error: Could not extract runtime"
  exit 1
fi

# Write TXT report
{
  echo "=== MolSim Benchmark Report ==="
  echo "Timestamp     : ${TS}"
  echo "Git commit    : ${GIT_SHA}"
  echo "Host          : ${HOST}"
  echo "Input file    : ${CFG}"
  echo "Input SHA1    : ${CFG_HASH}"
  echo "Runtime [s]   : ${RUNTIME}"
  echo
  echo "--- Raw Output ---"
  echo "${OUT}"
} > "${TXT_REPORT}"

# Write CSV report
{
  echo "timestamp,git_sha,host,input_file,input_sha1,runtime_seconds"
  echo "${TS},${GIT_SHA},${HOST},${CFG_BASENAME},${CFG_HASH},${RUNTIME}"
} > "${CSV_REPORT}"

echo "[benchmark] Reports written:"
echo "  ${TXT_REPORT}"
echo "  ${CSV_REPORT}"
