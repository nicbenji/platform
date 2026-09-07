#!/usr/bin/env bash
 
ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_PATH="$ROOT/build"
LIB_PATH="$ROOT/libs"
PROJECT_NAME="auzio"
MAIN_FILE="$ROOT/src/linux_auzio.c"
LIBS=""
COMPILE_FLAGS="-DEDITORE_DEBUG=1, -DWM_WAYLAND=1"

if [[ ! -d $BUILD_PATH ]]; then
    mkdir $BUILD_PATH
fi
cd $BUILD_PATH

clang $COMPILE_FLAGS \
    -Wall -Wextra -Wpedantic -Wshadow -Wsign-conversion -Wundef \
    -Wno-unused-parameter -Wno-gnu-anonymous-struct -Wno-missing-field-initializers \
    -Wno-unused-function -Wno-nested-anon-types -Wno-missing-braces -Wno-unused-variable \
    -Wno-macro-redefined -Wno-unused-but-set-variable -Wno-string-conversion \
    -g3 -ggdb -O0 -fbuiltin -fno-exceptions -fno-rtti -fno-omit-frame-pointer \
    $MAIN_FILE \
    -lGL -lglfw \
    -o $PROJECT_NAME.out

CLANGD_FILE="$ROOT/.clangd"
if [[ ! -f $CLANGD_FILE && $1 == "dev-setup" ]]; then
cat > $CLANGD_FILE << EOF
CompileFlags:
  Compiler: $(which clang)
  Add:
    - -include
    - $ROOT/unity_build.h
    - -DEDITORE_DEBUG=1
EOF
fi


