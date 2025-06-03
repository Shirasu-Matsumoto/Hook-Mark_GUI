@echo off
pushd %cd%
cd %~dp0\build
cmake ..\src
cmake --build .
popd
