#!/usr/bin/env bash
 
ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_PATH="$ROOT/build"
ASSET_PATH="$ROOT/assets"
PROJECT_NAME="platform_test"
MAIN_FILE="$ROOT/src/test_app.c"
LIBS=""
COMPILE_FLAGS="-DBUILD_DEBUG=1 -DWM_WAYLAND=1"
OPTIMIZATION_FLAGS="-g3 -ggdb -O0 -fbuiltin -fno-exceptions -fno-rtti -fno-omit-frame-pointer"
WARNINGS="-Wall -Wextra -pedantic -Wshadow -Wsign-conversion -Wundef
    -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter"

if [[ ! -d $BUILD_PATH ]]; then
    mkdir $BUILD_PATH
fi
cd $BUILD_PATH

clang $COMPILE_FLAGS $WARNINGS $OPTIMIZATION_FLAGS \
    "$ROOT/src/metagen/metagen.c" \
    -o metagen

cd $ASSET_PATH
    $BUILD_PATH/metagen
cd $BUILD_PATH

clang $COMPILE_FLAGS $WARNINGS $OPTIMIZATION_FLAGS $MAIN_FILE \
    -lGL \
    -o $PROJECT_NAME.out
