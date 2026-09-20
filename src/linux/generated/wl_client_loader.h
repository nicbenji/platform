#if !defined(WL_CLIENT_LOADER_H_)
#define WL_CLIENT_LOADER_H_

#define WL_DISPLAY_CONNECT(name) struct wl_display * name(const char * name)
typedef WL_DISPLAY_CONNECT(wl_display_connectFn);

#define WL_DISPLAY_GET_REGISTRY(name) struct wl_registry * name(struct wl_display * wl_display)
typedef WL_DISPLAY_GET_REGISTRY(wl_display_get_registryFn);

#define WL_REGISTRY_ADD_LISTENER(name) int name(struct wl_registry * wl_registry, const struct wl_registry_listener * listener, void * data)
typedef WL_REGISTRY_ADD_LISTENER(wl_registry_add_listenerFn);

#define WL_DISPLAY_ROUNDTRIP(name) int name(struct wl_display * display)
typedef WL_DISPLAY_ROUNDTRIP(wl_display_roundtripFn);

#define WL_COMPOSITOR_CREATE_SURFACE(name) struct wl_surface * name(struct wl_compositor * wl_compositor)
typedef WL_COMPOSITOR_CREATE_SURFACE(wl_compositor_create_surfaceFn);

typedef struct {
    wl_display_connectFn *display_connect;
    wl_display_get_registryFn *display_get_registry;
    wl_registry_add_listenerFn *registry_add_listener;
    wl_display_roundtripFn *display_roundtrip;
    wl_compositor_create_surfaceFn *compositor_create_surface;
} Wl_Functions;

internal B32 wl_load_functions(LibraryHandle wl_client_lib, Wl_Functions *wl);

#endif // WL_CLIENT_LOADER_H_