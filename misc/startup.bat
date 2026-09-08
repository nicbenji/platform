@echo off
subst W: "C:\Users\nicben\Coding\c\platform"
pushd W:

call vcvarsall.bat x64

wt -w 0 new-tab pwsh.exe -NoExit -Command "cd W:"

nvim .
