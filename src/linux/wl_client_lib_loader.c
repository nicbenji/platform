internal B32 wl_load_functions(LibraryHandle wl_client_lib, Wl_Functions *wl) {
    wl->display_connect = (Wl_DisplayConnectFn *)
        dynlib_load_proc(wl_client_lib, str8_lit("wl_display_connect"));
    if (!wl->display_connect) {
        return false;
    }

    return true;
}
