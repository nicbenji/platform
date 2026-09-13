ROOT="$(cd "$(dirname "$0")" && pwd)"
CLANGD_FILE="$ROOT/../.clangd"
if [[ ! -f $CLANGD_FILE ]]; then
cat > $CLANGD_FILE << EOF
CompileFlags:
  Compiler: $(which clang)
  Add:
    - -include
    - "$ROOT/../unity_build.h"
    - -DBUILD_DEBUG=1
EOF
fi


