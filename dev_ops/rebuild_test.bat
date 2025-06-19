@echo off
setlocal

echo 🧪 [rebuild_tests] Rebuilding test target: NoiseGeneratorTests...

cd ..\build-tests

cmake --build . --target NoiseGeneratorTests
if %errorlevel% neq 0 (
    echo Build failed. Exiting.
    exit /b %errorlevel%
)

echo Build succeeded.
echo Running tests...

ctest -V
pause