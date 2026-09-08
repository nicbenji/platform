#include "base_common.c"
#include "base_arena.c"
#include "base_strings.c"

#if OS_LINUX
#  include "../linux/linux_base.c"
#elif OS_WINDOWS
#  include "../win32/win32_base.c"
#else
#  error Missing platform layer for this OS
#endif
