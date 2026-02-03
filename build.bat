@echo off
REM Build script for Knoux Art Studio (Windows)

echo Building Knoux Art Studio...

if not exist build mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

echo Build complete!
echo Executable: build\Release\KnouxArtStudio.exe
pause
