#!/bin/bash
# Build script for Knoux Art Studio (Linux/macOS)

set -e

echo "Building Knoux Art Studio..."

mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu)

echo "Build complete!"
echo "Executable: build/KnouxArtStudio"
