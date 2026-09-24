internal B32 wl_load_functions(LibraryHandle wl_lib, Wl_Functions *wl){
    wl->display_connect = (wl_display_connectFn *)
        dynlib_load_proc(wl_lib, str8_lit("wl_display_connect"));
    if(!wl->display_connect) {
        return false;
    }

    wl->display_get_registry = (wl_display_get_registryFn *)
        dynlib_load_proc(wl_lib, str8_lit("wl_display_get_registry"));
    if(!wl->display_get_registry) {
        return false;
    }

    wl->registry_add_listener = (wl_registry_add_listenerFn *)
        dynlib_load_proc(wl_lib, str8_lit("wl_registry_add_listener"));
    if(!wl->registry_add_listener) {
        return false;
    }

    wl->display_roundtrip = (wl_display_roundtripFn *)
        dynlib_load_proc(wl_lib, str8_lit("wl_display_roundtrip"));
    if(!wl->display_roundtrip) {
        return false;
    }

    wl->compositor_create_surface = (wl_compositor_create_surfaceFn *)
        dynlib_load_proc(wl_lib, str8_lit("wl_compositor_create_surface"));
    if(!wl->compositor_create_surface) {
        return false;
    }

    return true;
}