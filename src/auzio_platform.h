#ifndef AUZIO_PLATFORM_H
#include "auzio_utils.h"

struct V2 {
    f32 x, y;
};

inline V2 createV2(f32 x, f32 y) {
    V2 result = {};
    result.x = x;
    result.y = y;
    return result;
}

struct AP_Memory {
    u64 permanent_memory_size;
    void *permanent_memory;

    u64 scratch_memory_size;
    void *scratch_memory;

    b32 is_initialized;
};

enum AP_ButtonId {
    // Numbers
    AP_BUTTON_ID_KEY_0,
    AP_BUTTON_ID_KEY_1,
    AP_BUTTON_ID_KEY_2,
    AP_BUTTON_ID_KEY_3,
    AP_BUTTON_ID_KEY_4,
    AP_BUTTON_ID_KEY_5,
    AP_BUTTON_ID_KEY_6,
    AP_BUTTON_ID_KEY_7,
    AP_BUTTON_ID_KEY_8,
    AP_BUTTON_ID_KEY_9,

    // Function keys
    AP_BUTTON_ID_KEY_F1,
    AP_BUTTON_ID_KEY_F2,
    AP_BUTTON_ID_KEY_F3,
    AP_BUTTON_ID_KEY_F4,
    AP_BUTTON_ID_KEY_F5,
    AP_BUTTON_ID_KEY_F6,
    AP_BUTTON_ID_KEY_F7,
    AP_BUTTON_ID_KEY_F8,
    AP_BUTTON_ID_KEY_F9,
    AP_BUTTON_ID_KEY_F10,
    AP_BUTTON_ID_KEY_F11,
    AP_BUTTON_ID_KEY_F12,

    // Letters
    AP_BUTTON_ID_KEY_A,
    AP_BUTTON_ID_KEY_B,
    AP_BUTTON_ID_KEY_C,
    AP_BUTTON_ID_KEY_D,
    AP_BUTTON_ID_KEY_E,
    AP_BUTTON_ID_KEY_F,
    AP_BUTTON_ID_KEY_G,
    AP_BUTTON_ID_KEY_H,
    AP_BUTTON_ID_KEY_I,
    AP_BUTTON_ID_KEY_J,
    AP_BUTTON_ID_KEY_K,
    AP_BUTTON_ID_KEY_L,
    AP_BUTTON_ID_KEY_M,
    AP_BUTTON_ID_KEY_N,
    AP_BUTTON_ID_KEY_O,
    AP_BUTTON_ID_KEY_P,
    AP_BUTTON_ID_KEY_Q,
    AP_BUTTON_ID_KEY_R,
    AP_BUTTON_ID_KEY_S,
    AP_BUTTON_ID_KEY_T,
    AP_BUTTON_ID_KEY_U,
    AP_BUTTON_ID_KEY_V,
    AP_BUTTON_ID_KEY_W,
    AP_BUTTON_ID_KEY_X,
    AP_BUTTON_ID_KEY_Y,
    AP_BUTTON_ID_KEY_Z,

    // Modifiers
    AP_BUTTON_ID_KEY_LEFT_SHIFT,
    AP_BUTTON_ID_KEY_RIGHT_SHIFT,
    AP_BUTTON_ID_KEY_LEFT_CONTROL,
    AP_BUTTON_ID_KEY_RIGHT_CONTROL,
    AP_BUTTON_ID_KEY_LEFT_ALT,
    AP_BUTTON_ID_KEY_RIGHT_ALT,
    AP_BUTTON_ID_KEY_CAPS_LOCK,

    // Common
    AP_BUTTON_ID_KEY_ESCAPE,
    AP_BUTTON_ID_KEY_BACKSPACE,
    AP_BUTTON_ID_KEY_DELETE,
    AP_BUTTON_ID_KEY_ENTER,
    AP_BUTTON_ID_KEY_TAB,
    AP_BUTTON_ID_KEY_SPACE,

    // Navigation
    AP_BUTTON_ID_KEY_LEFT,
    AP_BUTTON_ID_KEY_RIGHT,
    AP_BUTTON_ID_KEY_UP,
    AP_BUTTON_ID_KEY_DOWN,
    AP_BUTTON_ID_KEY_HOME,
    AP_BUTTON_ID_KEY_END,
    AP_BUTTON_ID_KEY_PAGE_UP,
    AP_BUTTON_ID_KEY_PAGE_DOWN,
    AP_BUTTON_ID_KEY_INSERT,

    // Mouse
    AP_BUTTON_ID_MOUSE_LEFT,
    AP_BUTTON_ID_MOUSE_MIDDLE,
    AP_BUTTON_ID_MOUSE_RIGHT,
    // TODO: what if more mouse buttons?

    AP_BUTTON_ID_COUNT
};

enum AP_InputEventType {
    AP_INPUT_EVENT_BUTTON_DOWN,
    AP_INPUT_EVENT_BUTTON_UP,
    AP_INPUT_EVENT_CODEPOINT
};

struct AP_InputEvent {
    AP_InputEventType type;
    union {
        AP_ButtonId button_id;

        int codepoint;
    };
};

#define MAX_INPUT_EVENTS 64

struct AP_Input {
    AP_InputEvent events[MAX_INPUT_EVENTS];
    int event_count;

    V2 mouse_pos;
    V2 mouse_move_delta;
    V2 mouse_wheel_delta;
};

enum AP_RenderableType {
    AP_RENDERABLE_TYPE_CLEAR,
    AP_RENDERABLE_TYPE_CODEPOINTS,
    AP_RENDERABLE_TYPE_RECTANGLE,
};

struct AP_Color {
    u8 r, g, b, a;
};

inline AP_Color ap_createColor(u8 r, u8 g, u8 b, u8 a) {
    AP_Color result = {};
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

struct AP_Rect {
    V2 top_left, bottom_right;
};

inline AP_Rect createRect(f32 width, f32 height, f32 x, f32 y) {
    AP_Rect result = {};
    result.top_left.x = x;
    result.top_left.y = y;
    result.bottom_right.x = x + width;
    result.bottom_right.y = y + height;
    return result;
}

typedef u32 AP_FontHandle;

// TODO: maybe compact this using caseys day 89 strat -> less mem usage
struct AP_Renderable {
    AP_RenderableType type;
    union {
        struct {
            AP_Color bg_color;
        };

        struct {
            AP_Color rect_color;
            AP_Rect rect;
        };

        struct {
            AP_FontHandle font_handle;
            int *codepoints;
            int codepoint_count;
            V2 pos;
            float font_size;
            float spacing;
            AP_Color font_color;
        };
    };
};

#define MAX_RENDERABLES 1024

struct AP_RenderBuffer {
    AP_Renderable renderables[MAX_RENDERABLES];
    int renderable_count;
};

internal void ap_editorUpdateAndRender(
    AP_Memory *memory,
    AP_Input *input,
    AP_RenderBuffer *render_buffer,
    AP_Rect screen_rect
);

#define AUZIO_PLATFORM_H
#endif
