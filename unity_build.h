#if !defined(UNITY_BUILD_H)
#define UNITY_BUILD_H

#include "src/base/base_inc.h"

#if OS_LINUX
#  include "src/linux/linux_inc.h"
#elif OS_WINDOWS
#  include "src/win32/win32_inc.h"
#endif

#endif // UNITY_BUILD_H

