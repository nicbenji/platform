global_var B32 g_should_close;

internal LRESULT w32_main_window_cb(
    HWND window, UINT message_code,
    WPARAM w_param, LPARAM l_param
) {
    LRESULT result = 0;

    switch (message_code) {
        case WM_SIZE: {
        } break;

        case WM_DESTROY: {
        } break;

        case WM_CLOSE: {
            g_should_close = true;
        } break;

        case WM_ACTIVATEAPP: {
            if (w_param == TRUE) {
                SetLayeredWindowAttributes(window, RGB(0,0,0), 255, LWA_ALPHA);
            } else {
                SetLayeredWindowAttributes(window, RGB(0,0,0), 64, LWA_ALPHA);
            }
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC device_ctx = BeginPaint(window, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(device_ctx, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW+1));

            EndPaint(window, &ps);
        } break;


        default: {
            result = DefWindowProcW(window, message_code, w_param, l_param);
        } break;


    }

    return result;
}

Wm_Window wm_open_main_window(MemoryArena *arena, Str8 app_name, U32 width, U32 height) {
    HMODULE instance = GetModuleHandleW(0);
    Str16 app_name16 = str16_from_str8(arena, app_name);

    // TODO: custom window title
    WNDCLASSW window_class = {};
    window_class.style = CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = w32_main_window_cb;
    window_class.hInstance = instance;
    window_class.lpszClassName = L"Win32PlatformWindowclassName";
    // windowClass.hIcon;

    if (!RegisterClassW(&window_class)) {
        // TODO: error handling + diagnostic
    }

    // TODO: WS_EX_APPWINDOW??
    HWND window_handle = CreateWindowExW(
        WS_EX_LAYERED,
        window_class.lpszClassName, (WCHAR *)app_name16.begin,
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        0, 0, instance, 0
    );

    if (!window_handle) {
        // TODO: error handling + diagnostic
    }

    Wm_Window result = { (U64)window_handle };
    g_should_close = false;
    return result;
}

// TODO: make should_close state local to a window?
internal B32 wm_window_should_close(Wm_Window window_handle) {
    return g_should_close;
}

internal void wm_process_events(Wm_Window window_handle) {
    for (MSG msg;
        PeekMessageW(&msg, (HWND)window_handle.u64[0], 0, 0, PM_REMOVE) > 0;
    ) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}
