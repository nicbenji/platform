#include "base_common.c"
#include "base_arena.c"
#include "base_math.c"
#include "base_thread_context.c"
#include "base_strings.c"
#include "base_files.c"

#if OS_WINDOWS
#  include "../os/win32/base_win32.c"
#elif OS_LINUX
#  include "../os/linux/base_linux.c"
#else
#  error Missing platform backend for this OS.
#endif
