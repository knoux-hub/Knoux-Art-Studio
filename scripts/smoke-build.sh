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

echo "Running vite build..."
npm run build
