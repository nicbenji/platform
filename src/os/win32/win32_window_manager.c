global_var W32_WM_State w32_wm_state;

internal WM_Backbuffer *wm_get_backbuffer(WM_Window window) {
    return &w32_wm_state.backbuffer;
}

internal Vec2_I32 w32_get_window_dimension(HWND window) {
    RECT client_rect;
    GetClientRect(window, &client_rect);

    I32 width = client_rect.right - client_rect.left;
    I32 height = client_rect.bottom - client_rect.top;
    Vec2_I32 result = vec2_i32(width, height);
    return result;
}

// TODO: probs give backbuffer as param
internal void w32_resize_bitmap_section(I32 width, I32 height) {
    // TODO: store and check buffer size to know if new allocation is needed?
    if (w32_wm_state.backbuffer.memory) {
        VirtualFree(w32_wm_state.backbuffer.memory, 0, MEM_RELEASE);
    }

    w32_wm_state.backbuffer.width = width;
    w32_wm_state.backbuffer.height = height;

    w32_wm_state.bitmap_info.bmiHeader.biSize = sizeof(w32_wm_state.bitmap_info.bmiHeader);
    w32_wm_state.bitmap_info.bmiHeader.biWidth = width;
    // NOTE: negative biHeight makes bitmap top left be P(0,0) and become positive downwards
    w32_wm_state.bitmap_info.bmiHeader.biHeight = -height;
    w32_wm_state.bitmap_info.bmiHeader.biPlanes = 1;
    w32_wm_state.bitmap_info.bmiHeader.biBitCount = (WORD)(w32_wm_state.backbuffer.bytes_per_pixel * 8);
    w32_wm_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

    // TODO: possible overflow
    int memory_size = (width * height) * w32_wm_state.backbuffer.bytes_per_pixel;
    w32_wm_state.backbuffer.memory = VirtualAlloc(0, memory_size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
}

// TODO: use parameters
internal void w32_display_buffer(HDC device_ctx, I32 x, I32 y, I32 width, I32 height) {
    Vec2_I32 dims = w32_get_window_dimension(w32_wm_state.w32_main_window.handle);

    StretchDIBits(
        device_ctx,
        0, 0, dims.x, dims.y,
        0, 0, w32_wm_state.backbuffer.width, w32_wm_state.backbuffer.height,
        w32_wm_state.backbuffer.memory,
        &w32_wm_state.bitmap_info,
        DIB_RGB_COLORS, SRCCOPY
    );
}

internal void wm_present_backbuffer(WM_Window window) {
    HWND window_handle = (HWND)window.u64[0];
    HDC device_ctx = GetDC(window_handle);
    w32_display_buffer(device_ctx, 0, 0, w32_wm_state.backbuffer.width, w32_wm_state.backbuffer.height);
    ReleaseDC(window_handle, device_ctx);
}


internal LRESULT w32_main_window_cb(
    HWND window, UINT message_code,
    WPARAM w_param, LPARAM l_param
) {
    LRESULT result = 0;

    switch (message_code) {
        default: {
            result = DefWindowProcW(window, message_code, w_param, l_param);
        } break;

        case WM_DESTROY: {
        } break;

        case WM_CLOSE: {
            wm_event_list_push(w32_wm_state.w32_wm_arena, &w32_wm_state.w32_main_window.events, WM_EventKind_WindowClose);
        } break;

        case WM_ACTIVATEAPP: {
            if (w_param == TRUE) {
                SetLayeredWindowAttributes(window, RGB(-1,0,0), 255, LWA_ALPHA);
            } else {
                SetLayeredWindowAttributes(window, RGB(0,0,0), 64, LWA_ALPHA);
            }
        } break;

        case WM_SIZE: {
            Vec2_I32 dims = w32_get_window_dimension(window);
            w32_resize_bitmap_section(dims.x, dims.y);
        } break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC device_ctx = BeginPaint(window, &ps);

            I32 x = ps.rcPaint.left;
            I32 y = ps.rcPaint.top;
            I32 width = ps.rcPaint.right - x;
            I32 height = ps.rcPaint.bottom - y;

            w32_display_buffer(device_ctx, x, y, width, height);

            EndPaint(window, &ps);
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            U32 vk_code = (U32)w_param;
            B32 was_down = (l_param & Bit(31)) != 0;
            B32 is_down = (l_param & Bit(32)) == 0;

            B32 released = false;
            if (was_down && !is_down) {
                released = true;
            }

            WM_Event *event = wm_event_list_push(
                w32_wm_state.w32_wm_arena, &w32_wm_state.w32_main_window.events,
                released ? WM_EventKind_KeyRelease : WM_EventKind_KeyPress
            );
            // TODO: NEXT: add keys
        } break;

    }

    return result;
}

internal WM_Window wm_open_main_window(Str8 title, U32 width, U32 height) {
    w32_wm_state.w32_wm_arena = mem_arena_default();
    w32_wm_state.backbuffer.bytes_per_pixel = 4;
    HMODULE instance = GetModuleHandleW(0);
    TempArena scratch = thread_ctx_scratch_begin(0, 0);
    Str16 app_name16 = str16_from_str8(scratch.arena, title);

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
    HWND window = CreateWindowExW(
        WS_EX_LAYERED|WS_EX_APPWINDOW,
        window_class.lpszClassName, (WCHAR *)app_name16.begin,
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        0, 0, instance, 0
    );

    if (!window) {
        // TODO: error handling + diagnostic
    }
    thread_ctx_scratch_end(scratch);

    w32_wm_state.w32_main_window.handle = window;
    WM_Window result = { (U64)window };
    return result;
}

// TODO: make this actually get the window via handle in future? per window events or global/per process?
internal WM_EventList wm_get_events(MemoryArena *arena, WM_Window window) {
    MemZeroStruct(&w32_wm_state.w32_main_window.events);

    for (MSG msg;
        PeekMessageW(&msg, (HWND)window.u64[0], 0, 0, PM_REMOVE) > 0;
    ) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        if (msg.message == WM_QUIT) {
            wm_event_list_push(arena, &w32_wm_state.w32_main_window.events, WM_EventKind_WindowClose);
        }
    }

    return w32_wm_state.w32_main_window.events;
}
