@echo off
setlocal

REM Set the build directories
set BUILD_DIR=%~dp0..\build
set BUILD_TESTS_DIR=%~dp0..\build-tests

REM Delete build directory
if exist "%BUILD_DIR%" (
    echo Deleting build directory...
    rmdir /s /q "%BUILD_DIR%"
    echo Build directory deleted successfully.
) else (
    echo Build directory does not exist. Nothing to clean.
)

REM Delete build-tests directory
if exist "%BUILD_TESTS_DIR%" (
    echo Deleting build-tests directory...
    rmdir /s /q "%BUILD_TESTS_DIR%"
    echo Build-tests directory deleted successfully.
) else (
    echo Build-tests directory does not exist. Nothing to clean.
)

pause
exit /b 0