@echo off

set "ROOT=%~dp0"
set compilerFlags=-nologo -MTd -Gm- -GR- -EHa- -Od -FC -Z7
set warnings=-W4 -wd4201 -wd4100 -wd4189
set buildFlags=-DBUILD_DEBUG=1
REM TODO: dynamically link instead? -> only needed symbols
set linkerFlags=-opt:ref -incremental:no User32.lib Gdi32.lib
set optimizationFlags=-O2 -Oi -fp:fast
set buildPath="%ROOT%build"
set assetPath="%ROOT%assets"

IF NOT EXIST %buildPath% mkdir %buildPath%
pushd %buildPath%

REM 64-bit build
cl %compilerFlags% -DBUILD_CLI=1 %warnings% %buildFlags% -Fmmetagen.map "../src/metagen/metagen.c" -link %linkerFlags%

pushd %assetPath%
    ..\build\metagen.exe
popd

cl %compilerFlags% %warnings% %buildFlags% -Fmtest_app.map "../src/test_app.c" -link %linkerFlags%
popd

