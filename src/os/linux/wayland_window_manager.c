typedef enum {
    Wl_SurfaceState_None,
    Wl_SurfaceState_AckedConfigure,
    Wl_SurfaceState_Attached,
} Wl_SurfaceState;

typedef struct {
    U32 wl_registry;
    U32 wl_shm;
    U32 wl_shm_pool;
    U32 wl_buffer;
    U32 xdg_wm_base;
    U32 xdg_surface;
    U32 wl_compositor;
    U32 wl_surface;
    U32 xdg_toplevel;

    U32 stride;
    U32 width;
    U32 height;

    U32 shm_pool_size;
    U8 *shm_pool_data;

    Wl_SurfaceState surface_state;
} Wl_State;

global_var U32 wl_curr_id = 0;

#define WlBufWrite(T, buf, buf_size, val) do { \
    *((T *)(buf)) = (val); \
    (buf_size) += sizeof(T); \
} while(0)

internal int wl_init(void) {
    // TODO: factor this out somewhere? wl init code/extra struct? static init? comp time?
    U32 wl_display_object_id = 1;
    U16 wl_registry_event_global = 0;
    U16 wl_shm_pool_event_format = 0;
    U16 wl_buffer_event_release = 0;
    U16 wl_xdg_wm_base_event_ping = 0;
    U16 wl_xdg_toplevel_event_configure = 0;
    U16 wl_xdg_toplevel_event_close = 1;
    U16 wl_xdg_surface_event_configure = 0;
    U16 wl_display_get_registry_opcode = 1;
    U16 wl_registry_bind_opcode = 0;
    U16 wl_compositor_create_surface_opcode = 0;
    U16 wl_xdg_wm_base_pong_opcode = 3;
    U16 wl_xdg_surface_ack_configure_opcode = 4;
    U16 wl_shm_create_pool_opcode = 0;
    U16 wl_xdg_wm_base_get_xdg_surface_opcode = 2;
    U16 wl_shm_pool_create_buffer_opcode = 0;
    U16 wl_surface_attach_opcode = 1;
    U16 wl_xdg_surface_get_toplevel_opcode = 1;
    U16 wl_surface_commit_opcode = 6;
    U16 wl_display_error_event = 0;
    U32 wl_format_argb8888 = 0;
    U32 wl_header_size = 8;
    U32 wl_color_channels = 4;


    const char *xdg_runtime_dir_cstr = getenv("XDG_RUNTIME_DIR");
    if (!xdg_runtime_dir_cstr) {
        // TODO: error handling + diagnostic
        return 1;
    }

    Str8 xdg_runtime_dir = str8_from_cstr(xdg_runtime_dir_cstr);

    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    Assert(xdg_runtime_dir.length <= CStrLen(addr.sun_path)); // TODO: logging
    U64 socket_path_len = 0;

    MemCopy(addr.sun_path, xdg_runtime_dir.begin, xdg_runtime_dir.length);
    socket_path_len += xdg_runtime_dir.length;
    addr.sun_path[socket_path_len++] = '/';

    const char *wl_display_cstr = getenv("WAYLAND_DISPLAY");
    Str8 wl_display = str8_zero();
    if (!wl_display_cstr) {
        wl_display = str8_lit("wayland-0");
    } else {
        wl_display = str8_from_cstr(wl_display_cstr);
    }
    MemCopy(addr.sun_path + socket_path_len,
            wl_display.begin, wl_display.length);
    socket_path_len += wl_display.length;

    // TODO: nonblocking/async IO?
    int wl_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (wl_fd == -1) {
        // TODO: error handling
        return 1;
    }
    B32 connected = connect(wl_fd, (struct sockaddr *)&addr, sizeof(addr)) != -1;
    if (!connected) {
        // TODO: error handling
        return 1;
    }

    // Get wl_registry
    U64 msg_size = 0;
    char msg[128];
    WlBufWrite(U32, msg, msg_size, wl_display_object_id);
    WlBufWrite(U16, msg, msg_size, wl_display_get_registry_opcode);

    U16 msg_announce_size = wl_header_size + sizeof(wl_curr_id);
    WlBufWrite(U16, msg, msg_size, msg_announce_size);

    ++wl_curr_id;
    WlBufWrite(U32, msg, msg_size, wl_curr_id);

    if (send(wl_fd, msg, msg_size, MSG_DONTWAIT) != (S64)msg_size) {
        return 1;
    }

    printf("-> wl_display@%u.get_registry: wl_registry=%u\n",
         wl_display_object_id, wl_curr_id);

    Wl_State wl_state = {0};
    wl_state.wl_registry = wl_curr_id;
    wl_state.width = 1280;
    wl_state.height = 720;
    wl_state.stride = wl_state.width * wl_color_channels;
    wl_state.shm_pool_size = wl_state.height * wl_state.stride;

    char display_buf_name[] = "/wayland-window"; // TODO: why do tutorials pick a random name?
    int shm_fd = shm_open(display_buf_name, O_RDWR|O_CREAT|O_EXCL, 0600);
    if (shm_fd == -1) {
        // TODO: error handling
        return 1;
    }

    shm_unlink(display_buf_name); // TODO: validate if this succeeded and retry?????
    if (ftruncate(shm_fd, wl_state.shm_pool_size) == -1) {
        // TODO: error handling
        return 1;
    }
    wl_state.shm_pool_data = mmap(
        0, wl_state.shm_pool_size,
        PROT_READ|PROT_WRITE, MAP_SHARED,
        shm_fd, 0
    );
    if (wl_state.shm_pool_data == MAP_FAILED) {
        // TODO: error handling
        return 1;
    }

    return 0;
}
