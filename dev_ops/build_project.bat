@echo off
setlocal enabledelayedexpansion

REM Prompt user for configuration
echo =================================================================
echo Select build configuration:
echo [1] Debug
echo [2] Release
echo [3] Testing (test-only mode)
echo =================================================================
set /p choice=Enter your choice (1, 2 or 3): 

REM Determine config and optional flags
set EXTRA_FLAGS=
set BUILD_SUBDIR=build

if "%choice%"=="1" (
    set CONFIG=Debug
) else if "%choice%"=="2" (
    set CONFIG=Release
) else if "%choice%"=="3" (
    set CONFIG=Testing
    set EXTRA_FLAGS=-DBUILD_TESTING_ONLY=ON
    set BUILD_SUBDIR=build-tests
) else (
    echo Invalid choice.
    pause
    exit /b 1
)

REM Set paths
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..
set BUILD_DIR=%PROJECT_ROOT%\%BUILD_SUBDIR%

REM Create build directory if it doesn't exist
if not exist "!BUILD_DIR!" (
    mkdir "!BUILD_DIR!"
)

REM Go to build directory
cd /d "!BUILD_DIR!"

REM Generate Visual Studio project
cmake .. -G "Visual Studio 17 2022" ^
    -DCMAKE_CONFIGURATION_TYPES=%CONFIG% ^
    %EXTRA_FLAGS%

if errorlevel 1 (
    echo Failed to generate CMake project.
    pause
    exit /b 1
)

REM Build project in selected configuration
cmake --build . --config %CONFIG%

if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

echo %CONFIG% build completed successfully.
pause
exit /b 0