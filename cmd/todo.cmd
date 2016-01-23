@echo off

rem this is where todo.md will be stored if the file doesn't exist locally
set folder=%USERPROFILE%\OneDrive\Documents

if exist todo.md (
  "%~dp0\..\bin\todo" %*
) else (
  setlocal & cd %folder% && "%~dp0\..\bin\todo" %* & endlocal
)
