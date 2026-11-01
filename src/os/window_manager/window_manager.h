typedef struct {
    U64 u64[1];
} WM_Window;

typedef struct {
    U8 *memory;
    int bytes_per_pixel;
    int width;
    int height;
} WM_Backbuffer;

typedef enum {
    WM_EventKind_WindowClose,
    WM_EventKind_KeyRelease,
    WM_EventKind_KeyPress,
    WM_EventKind_COUNT,
} WM_EventKind;

typedef struct WM_Event WM_Event;
struct WM_Event {
    WM_Event *next;
    WM_EventKind kind;
};

typedef struct {
    WM_Event *first;
    WM_Event *last;
} WM_EventList;

internal WM_Window wm_window_zero(void);

internal WM_Event *wm_event_list_push(MemoryArena *arena, WM_EventList *events, WM_EventKind kind);

// NOTE: OS-specific procedures
// TODO: implement a close window if support for multiple os windows is added
internal WM_Window wm_open_main_window(Str8 title, U32 width, U32 height);
internal WM_EventList wm_get_events(MemoryArena *arena, WM_Window window);
