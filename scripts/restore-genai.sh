#!/usr/bin/env bash
set -euo pipefail

echo "Restoring @google/genai dependency..."
echo "If this fails with E403/ETARGET, ensure registry access is available."

npm install @google/genai@0.2.1

echo "Dependency restored. Replace mock import with @google/genai."
