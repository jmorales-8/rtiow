@echo off

cd ../

if not exist "output" mkdir output

build\VisualStudio\Release\rtiow.exe > output/output.ppm

pause
