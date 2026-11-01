#ifndef WIN32_WINDOW_MANAGER_H
#define WIN32_WINDOW_MANAGER_H

typedef struct {
    HWND handle;
    WM_EventList events;
} W32_WM_Window;

typedef struct {
    int width;
    int height;
} W32_WindowDimension;

typedef struct {
    MemoryArena *w32_wm_arena;
    W32_WM_Window w32_main_window;

    BITMAPINFO bitmap_info;
    WM_Backbuffer backbuffer;
} W32_WM_State;

#endif  // WIN32_WINDOW_MANAGER_H
