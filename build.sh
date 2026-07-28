#!/usr/bin/env bash
 
ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_PATH="$ROOT/build"
LIB_PATH="$ROOT/libs"
MAIN_FILE="$ROOT/src/auzio_linux.cpp"
LIBS=""
COMPILE_FLAGS="-DEDITORE_DEBUG=1"

if [[ ! -d $BUILD_PATH ]]; then
    mkdir $BUILD_PATH
fi
cd $BUILD_PATH

# "$ROOT/build_raylib.sh"

clang++ $COMPILE_FLAGS \
    -Wall -Wextra -Werror -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wundef \
    -Wno-unused-parameter -Wno-gnu-anonymous-struct -Wno-missing-field-initializers \
    -Wno-unused-function -Wno-nested-anon-types -Wno-missing-braces -Wno-unused-variable \
    -Wno-macro-redefined -Wno-unused-but-set-variable -Wno-string-conversion \
    -g3 -ggdb -O0 -fbuiltin -fno-exceptions -fno-rtti -fno-omit-frame-pointer \
    -isystem "$LIB_PATH/raylib/src" \
    $MAIN_FILE \
    "$BUILD_PATH/raylib/raylib/libraylib.a" \
    -lm -lX11 \
    -o auzio.out
