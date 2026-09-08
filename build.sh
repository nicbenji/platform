#!/usr/bin/env bash
 
ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_PATH="$ROOT/build"
LIB_PATH="$ROOT/libs"
PROJECT_NAME="platform_test"
MAIN_FILE="$ROOT/src/test_app.c"
LIBS=""
COMPILE_FLAGS="-DBUILD_DEBUG=1 -DWM_WAYLAND=1 -D_FILE_OFFSET_BITS=64"
OPTIMIZATION_FLAGS="-g3 -ggdb -O0 -fbuiltin -fno-exceptions -fno-rtti -fno-omit-frame-pointer"
WARNINGS="-Wall -Wextra -Wpedantic -Wshadow -Wsign-conversion -Wundef
    -Wno-unused-parameter -Wno-gnu-anonymous-struct -Wno-missing-field-initializers
    -Wno-unused-function -Wno-nested-anon-types -Wno-missing-braces -Wno-unused-variable
    -Wno-macro-redefined -Wno-unused-but-set-variable -Wno-string-conversion"

if [[ ! -d $BUILD_PATH ]]; then
    mkdir $BUILD_PATH
fi
cd $BUILD_PATH

clang $COMPILE_FLAGS $WARNINGS $OPTIMIZATION_FLAGS \
    "$ROOT/src/metagen.c" \
    -o metagen
./metagen

clang $COMPILE_FLAGS $WARNINGS $OPTIMIZATION_FLAGS $MAIN_FILE \
    -lGL \
    -o $PROJECT_NAME.out

CLANGD_FILE="$ROOT/.clangd"
if [[ ! -f $CLANGD_FILE && $1 == "dev-setup" ]]; then
cat > $CLANGD_FILE << EOF
CompileFlags:
  Compiler: $(which clang)
  Add:
    - -include
    - $ROOT/unity_build.h
    - -DBUILD_DEBUG=1
EOF
fi


