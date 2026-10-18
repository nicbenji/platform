typedef enum {
    Wl_SurfaceState_None,
    Wl_SurfaceState_AckedConfigure,
    Wl_SurfaceState_Attached,
} WL_SurfaceState;

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

    WL_SurfaceState surface_state;
} WL_State;
