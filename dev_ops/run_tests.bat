@echo off
setlocal

cd ..\build-tests

echo Running tests with CTest...
ctest -V

pause