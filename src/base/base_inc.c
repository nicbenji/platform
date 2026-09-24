#include "base_common.c"
#include "base_arena.c"
#include "base_thread_context.c"
#include "base_strings.c"
#include "base_files.c"

#if OS_LINUX
#  include "../linux/base_linux.c"
#elif OS_WINDOWS
#  include "../win32/base_win32.c"
#else
#  error Missing platform layer for this OS
#endif
