@echo off

:: Navigate to root directory of repository
cd /d %~dp0
cd ..\..

setlocal

:: Copy properties as environment variables
FOR /F "tokens=1,2 delims==" %%G IN (library.properties) DO (set %%G=%%H) 
echo Found library.name=%name%
echo Found library.version=%version%
set library_fullname=%name%-%version%

::Copy content to temporary folder.
set outdir=%TEMP%\%library_fullname%
IF EXIST %outdir% (
  rmdir /S /Q %outdir%
)
mkdir %outdir% 2>NUL 1>NUL
xcopy /s /y *.* %outdir%

::Zip content
"C:\Program Files\7-Zip\7z" a %outdir%.zip %outdir%

::Open an explorer windows with the generated file selected
start "" explorer.exe /select,"%outdir%.zip"

endlocal
