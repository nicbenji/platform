#if !defined(UNITY_BUILD_H)
#define UNITY_BUILD_H

// Include file to make lsp not complain (too much)

#include "src/base/base_inc.h"
#include "src/metagen/metagen_inc.h"

#if OS_LINUX
#  include "src/os/linux/base_linux.h"
#elif OS_WINDOWS
#  include "src/os/win32/base_win32.h"
#endif

#endif // UNITY_BUILD_H

