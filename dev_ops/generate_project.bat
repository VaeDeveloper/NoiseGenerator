@echo off
setlocal

REM Set the root path of the project
set PROJECT_ROOT=%~dp0..
cd /d %PROJECT_ROOT%

REM Set the build directory path
set BUILD_DIR=%PROJECT_ROOT%\build

REM Create the build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

cd /d "%BUILD_DIR%"

REM Generate the project files using CMake
cmake .. -G "Visual Studio 17 2022"

if %errorlevel% neq 0 (
    echo Failed to generate CMake project files.
    pause
    exit /b 1
)

echo CMake project successfully generated in the build\ folder.
pause
exit /b 0