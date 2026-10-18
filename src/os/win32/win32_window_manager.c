global_var W32_WM_Window g_w32_main_window;
global_var MemoryArena *g_w32_wm_arena;

internal Vec2_I32 w32_get_window_dimension(HWND window_handle) {
    RECT client_rect;
    GetClientRect(window_handle, &client_rect);

    I32 width = client_rect.right - client_rect.left;
    I32 height = client_rect.bottom - client_rect.top;
    Vec2_I32 result = vec2_i32(width, height);
    return result;
}

internal void w32_resize_bitmap_section(void) {

}

internal void w32_display_buffer(void) {

}

internal LRESULT w32_main_window_cb(
    HWND window, UINT message_code,
    WPARAM w_param, LPARAM l_param
) {
    LRESULT result = 0;

    switch (message_code) {
        case WM_DESTROY: {
        } break;

        case WM_CLOSE: {
            wm_event_list_push(g_w32_wm_arena, &g_w32_main_window.events, WM_EventKind_WindowClose);
        } break;

        case WM_ACTIVATEAPP: {
            if (w_param == TRUE) {
                SetLayeredWindowAttributes(window, RGB(-1,0,0), 255, LWA_ALPHA);
            } else {
                SetLayeredWindowAttributes(window, RGB(0,0,0), 64, LWA_ALPHA);
            }
        } break;

        case WM_SIZE: {
            w32_resize_bitmap_section();
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC device_ctx = BeginPaint(window, &ps);

            w32_display_buffer();

            EndPaint(window, &ps);
        } break;


        default: {
            result = DefWindowProcW(window, message_code, w_param, l_param);
        } break;


    }

    return result;
}

WM_Window wm_open_main_window(Str8 app_name, U32 width, U32 height) {
    g_w32_wm_arena = mem_arena_default();
    HMODULE instance = GetModuleHandleW(0);
    TempArena scratch = thread_ctx_scratch_begin(0, 0);
    Str16 app_name16 = str16_from_str8(scratch.arena, app_name);

    // TODO: custom window title
    WNDCLASSW window_class = {0};
    window_class.style = CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = w32_main_window_cb;
    window_class.hInstance = instance;
    window_class.lpszClassName = L"Win32PlatformWindowclassName";
    // windowClass.hIcon;

    if (!RegisterClassW(&window_class)) {
        // TODO: error handling + diagnostic
    }

    // TODO: WS_EX_LAYERED should rly be used??
    HWND window_handle = CreateWindowExW(
        WS_EX_LAYERED|WS_EX_APPWINDOW,
        window_class.lpszClassName, (WCHAR *)app_name16.begin,
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        0, 0, instance, 0
    );

    if (!window_handle) {
        // TODO: error handling + diagnostic
    }
    thread_ctx_scratch_end(scratch);

    g_w32_main_window.handle = window_handle;
    WM_Window result = { (U64)window_handle };
    return result;
}

// TODO: make this actually get the window via handle in future
internal WM_EventList wm_get_events(MemoryArena *arena, WM_Window window) {
    MemZeroStruct(&g_w32_main_window.events);

    for (MSG msg;
        PeekMessageW(&msg, (HWND)window.u64[0], 0, 0, PM_REMOVE) > 0;
    ) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        if (msg.message == WM_QUIT) {
            wm_event_list_push(arena, &g_w32_main_window.events, WM_EventKind_WindowClose);
        }
    }
    return g_w32_main_window.events;
}
