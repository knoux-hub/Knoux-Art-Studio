@echo off
echo 🏛️ KNOUX ART STUDIO - SOVEREIGN BUILD SYSTEM
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
echo 🚀 BUILD COMPLETED ON DRIVE F:
pause
