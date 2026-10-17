typedef struct {
  U64 u64[1];
} Wm_Window;

internal Wm_Window wm_open_main_window(MemoryArena *arena, Str8 app_name, U32 width, U32 height);
internal B32 wm_window_should_close(Wm_Window window_handle);
internal void wm_process_events(Wm_Window window_handle);
