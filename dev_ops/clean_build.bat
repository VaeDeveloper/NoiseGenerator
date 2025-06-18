@echo off
setlocal

REM Set the build directory path
set BUILD_DIR=%~dp0..\build

REM Check if the build directory exists
if exist "%BUILD_DIR%" (
    echo Deleting build directory...
    rmdir /s /q "%BUILD_DIR%"
    echo Build directory deleted successfully.
) else (
    echo ℹBuild directory does not exist. Nothing to clean.
)

pause
exit /b 0