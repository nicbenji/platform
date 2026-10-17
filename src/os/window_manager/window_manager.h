typedef struct {
  U64 u64[1];
} Wm_Window;

Wm_Window *wm_open_main_window(Str8 app_name, U32 width, U32 height);
