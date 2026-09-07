#if !defined (WL_CLIENT_LOADER_H)
#define WL_CLIENT_LOADER_H

typedef struct wl_display Wl_Display;
typedef Wl_Display *(Wl_DisplayConnectFn)(const char *name);

typedef struct {
    Wl_DisplayConnectFn *display_connect;
} Wl_Funcs;

internal B32 wl_load_funcs(void *wl_client_lib, Wl_Funcs *wl);

#endif // WL_CLIENT_LOADER_H
