@echo off

set compilerFlags=-nologo -MTd -Gm- -GR- -EHa- -Od -FC -Z7
set warnings=-W4 -wd4201 -wd4100 -wd4189
set buildFlags=-DBUILD_DEBUG=1
set linkerFlags=-opt:ref -incremental:no
set optimizationFlags=-O2 -Oi -fp:fast
set buildPath="./build"

IF NOT EXIST %buildPath% mkdir %buildPath%
pushd %buildPath%

REM 64-bit build
cl %compilerFlags% %warnings% %buildFlags% -Fmmetagen.map "../src/metagen.c" -link %linkerFlags%
.\metagen.exe

cl %compilerFlags% %warnings% %buildFlags% -Fmtest_app.map "../src/test_app.c" -link %linkerFlags%
popd

