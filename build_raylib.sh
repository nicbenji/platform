# TODO: this installs globally, vendor into project?
#!/usr/bin/env bash

ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$ROOT/build"
RAYLIB_SRC="$ROOT/libs/raylib"
RAYLIB_BUILD="$BUILD_DIR/raylib"

mkdir -p "$RAYLIB_BUILD"

cmake -S "$RAYLIB_SRC" -B "$RAYLIB_BUILD" \
    -DGLFW_BUILD_WAYLAND=ON \
    -DBUILD_SHARED_LIBS=OFF

cmake --build "$RAYLIB_BUILD" -j$(nproc)
