@echo off

set "ROOT=%~dp0"
set compilerFlags=-nologo -MTd -Gm- -GR- -EHa- -Od -FC -Z7
set warnings=-W4 -wd4201 -wd4100 -wd4189
set buildFlags=-DBUILD_DEBUG=1
set linkerFlags=-opt:ref -incremental:no User32.lib
set optimizationFlags=-O2 -Oi -fp:fast
set buildPath="%ROOT%build"
set assetPath="%ROOT%assets"

IF NOT EXIST %buildPath% mkdir %buildPath%
pushd %buildPath%

REM 64-bit build
<<<<<<< Updated upstream
cl %compilerFlags% %warnings% %buildFlags% -Fmmetagen.map "../src/metagen/metagen.c" -DBUILD_CLI=1 -link %linkerFlags%
.\metagen.exe
=======
cl %compilerFlags% -DBUILD_CLI=1 %warnings% %buildFlags% -Fmmetagen.map "../src/metagen/metagen.c" -link %linkerFlags%

pushd %assetPath%
    .\metagen.exe
popd
>>>>>>> Stashed changes

cl %compilerFlags% %warnings% %buildFlags% -Fmtest_app.map "../src/test_app.c" -link %linkerFlags%
popd

