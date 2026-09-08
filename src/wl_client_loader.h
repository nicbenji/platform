#if !defined (WL_CLIENT_LOADER_H_)
#define WL_CLIENT_LOADER_H_

typedef struct wl_display Wl_Display;
#define WL_DISPLAY_CONNECT(name) Wl_Display *name(const char *name)
typedef WL_DISPLAY_CONNECT(Wl_DisplayConnectFn);

typedef struct {
    Wl_DisplayConnectFn *display_connect;
} Wl_Funcs;

internal B32 wl_load_funcs(void *wl_client_lib, Wl_Funcs *wl);

#endif // WL_CLIENT_LOADER_H_
