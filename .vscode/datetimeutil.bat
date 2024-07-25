for /F "tokens=2" %%i in ('date /t') do set mydate=%%i
set mytime=%time::=%
set mytime=%mytime:.=%
set mytime=%mytime: =0%
set mydate=%mydate:/=%
echo %mydate%%mytime%