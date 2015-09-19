@echo off

rem this is where todo.txt will be stored
set folder=%USERPROFILE%\OneDrive\Documents

pushd %folder%
"%~dp0\..\bin\todo" %*
popd
