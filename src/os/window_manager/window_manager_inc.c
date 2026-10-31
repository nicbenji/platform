#include "window_manager.c"

#if OS_WINDOWS
#  include "../win32/win32_window_manager.c"
#else
#  error No window manager for this OS.
#endif
