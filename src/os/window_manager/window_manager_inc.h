#ifndef WINDOW_MANAGER_INC_H_
#define WINDOW_MANAGER_INC_H_

#include "window_manager.h"

#if OS_WINDOWS
#  include "../win32/win32_window_manager.h"
#elif OS_LINUX
#  include "../linux/wayland_window_manager.h"
#else
#  error No window manager for this OS.
#endif

#endif  // WINDOW_MANAGER_INC_H_
