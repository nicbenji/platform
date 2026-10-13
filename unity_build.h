#if !defined(UNITY_BUILD_H)
#define UNITY_BUILD_H

#include "src/base/base_inc.h"
#include "src/metagen/metagen_inc.h"

#if OS_LINUX
#  include "src/linux/base_linux.h"
#elif OS_WINDOWS
#  include "src/win32/base_win32.h"
#endif

#endif // UNITY_BUILD_H

