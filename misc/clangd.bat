@echo off

set "ROOT=%~dp0"
for %%A in ("%ROOT%..\unity_build.h") do set "UNITY_BUILD=%%~fA"
set "UNITY_BUILD=%UNITY_BUILD:\=/%"

set "CLANGD_FILE=%ROOT%..\.clangd"

if not exist "%CLANGD_FILE%" (
    (
        echo CompileFlags:
        echo   Compiler: cl.exe
        echo   Add:
        echo     - -FI%UNITY_BUILD%
        echo     - -DBUILD_DEBUG=1
    ) > "%CLANGD_FILE%"
)
