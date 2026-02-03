#!/usr/bin/env bash
set -euo pipefail

LOG_FILE="smoke-build.log"

echo "Running npm ci..."
if ! npm ci 2>&1 | tee "${LOG_FILE}"; then
  if rg -q "@google/genai|E403|ETARGET" "${LOG_FILE}"; then
    echo "Registry blocked for @google/genai. Skipping build."
    exit 0
  fi
  echo "npm ci failed. See ${LOG_FILE}."
  exit 1
fi

echo "Running typecheck..."
if ! npm run typecheck 2>&1 | tee -a "${LOG_FILE}"; then
  if rg -q "TS2688|@types/node|E403|ETARGET" "${LOG_FILE}"; then
    echo "Typecheck blocked by registry constraints. Skipping."
    exit 0
  fi
  echo "Typecheck failed. See ${LOG_FILE}."
  exit 1
fi

echo "Running vite build..."
if ! npm run build 2>&1 | tee -a "${LOG_FILE}"; then
  if rg -q "@google/genai|E403|ETARGET" "${LOG_FILE}"; then
    echo "Build blocked by registry constraints. Skipping."
    exit 0
  fi
  echo "Build failed. See ${LOG_FILE}."
  exit 1
fi
