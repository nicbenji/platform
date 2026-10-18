#ifndef BASE_INC_H_
#define BASE_INC_H_

#include "base_platform_context.h"
#include "base_common.h"
#include "base_linked_list.h"

#include "base_memory.h"
#include "base_arena.h"
#include "base_math.h"
#include "base_strings.h"
#include "base_thread_context.h"
#include "base_entrypoint.h"
#include "base_dynamic_libs.h"
#include "base_files.h"

#if OS_WINDOWS
    #include "../os/win32/base_win32.h"
#elif OS_LINUX
    #include "../os/linux/base_linux.h"
#else
    #error Missing platform backend for this OS.
#endif

#endif  // BASE_INC_H_

