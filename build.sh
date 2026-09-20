#!/usr/bin/env bash
 
ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_PATH="$ROOT/build"
LIB_PATH="$ROOT/libs"
PROJECT_NAME="platform_test"
MAIN_FILE="$ROOT/src/test_app.c"
LIBS=""
COMPILE_FLAGS="-DBUILD_DEBUG=1 -DWM_WAYLAND=1"
OPTIMIZATION_FLAGS="-g3 -ggdb -O0 -fbuiltin -fno-exceptions -fno-rtti -fno-omit-frame-pointer"
WARNINGS="-Wall -Wextra -Wpedantic -Wshadow -Wsign-conversion -Wundef
    -Wno-unused-parameter -Wno-gnu-anonymous-struct -Wno-missing-field-initializers
    -Wno-unused-function -Wno-nested-anon-types -Wno-missing-braces -Wno-unused-variable
    -Wno-unused-but-set-variable -Wno-string-conversion"

if [[ ! -d $BUILD_PATH ]]; then
    mkdir $BUILD_PATH
fi
cd $BUILD_PATH

clang $COMPILE_FLAGS $WARNINGS $OPTIMIZATION_FLAGS \
    "$ROOT/src/metagen/metagen.c" \
    -o metagen
./metagen

clang $COMPILE_FLAGS $WARNINGS $OPTIMIZATION_FLAGS $MAIN_FILE \
    -lGL \
    -o $PROJECT_NAME.out
