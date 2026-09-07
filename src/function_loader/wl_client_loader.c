internal B32 wl_load_funcs(void *wl_client_lib, Wl_Funcs *wl) {
    wl->display_connect = (Wl_DisplayConnectFn *)dlsym(wl_client_lib, "wl_display_connect");
    if (!wl->display_connect) {
        return false;
    }

    return true;
}
