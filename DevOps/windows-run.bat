@echo off

for /F "tokens=2" %%i in ('date /t') do set mydate=%%i
set mytime=%time::=%
set mytime=%mytime:.=%
set mydate=%mydate:/=%

cd ../

if not exist "output" mkdir output

build\VisualStudio\Release\rtiow.exe > output/output%mydate%%mytime%.ppm

pause
