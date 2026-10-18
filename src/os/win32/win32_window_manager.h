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


#endif  // WIN32_WINDOW_MANAGER_H
