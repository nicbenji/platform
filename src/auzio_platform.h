#ifndef AUZIO_PLATFORM_H
#include "auzio_utils.h"

struct EditorMemory {
    u64 permanent_memory_size;
    void *permanent_memory;

    u64 scratch_memory_size;
    void *scratch_memory;

    b32 is_initialized;
};

enum EditorButtonId {
    // Numbers
    EDITOR_BUTTON_ID_KEY_0,
    EDITOR_BUTTON_ID_KEY_1,
    EDITOR_BUTTON_ID_KEY_2,
    EDITOR_BUTTON_ID_KEY_3,
    EDITOR_BUTTON_ID_KEY_4,
    EDITOR_BUTTON_ID_KEY_5,
    EDITOR_BUTTON_ID_KEY_6,
    EDITOR_BUTTON_ID_KEY_7,
    EDITOR_BUTTON_ID_KEY_8,
    EDITOR_BUTTON_ID_KEY_9,

    // Function keys
    EDITOR_BUTTON_ID_KEY_F1,
    EDITOR_BUTTON_ID_KEY_F2,
    EDITOR_BUTTON_ID_KEY_F3,
    EDITOR_BUTTON_ID_KEY_F4,
    EDITOR_BUTTON_ID_KEY_F5,
    EDITOR_BUTTON_ID_KEY_F6,
    EDITOR_BUTTON_ID_KEY_F7,
    EDITOR_BUTTON_ID_KEY_F8,
    EDITOR_BUTTON_ID_KEY_F9,
    EDITOR_BUTTON_ID_KEY_F10,
    EDITOR_BUTTON_ID_KEY_F11,
    EDITOR_BUTTON_ID_KEY_F12,

    // Letters
    EDITOR_BUTTON_ID_KEY_A,
    EDITOR_BUTTON_ID_KEY_B,
    EDITOR_BUTTON_ID_KEY_C,
    EDITOR_BUTTON_ID_KEY_D,
    EDITOR_BUTTON_ID_KEY_E,
    EDITOR_BUTTON_ID_KEY_F,
    EDITOR_BUTTON_ID_KEY_G,
    EDITOR_BUTTON_ID_KEY_H,
    EDITOR_BUTTON_ID_KEY_I,
    EDITOR_BUTTON_ID_KEY_J,
    EDITOR_BUTTON_ID_KEY_K,
    EDITOR_BUTTON_ID_KEY_L,
    EDITOR_BUTTON_ID_KEY_M,
    EDITOR_BUTTON_ID_KEY_N,
    EDITOR_BUTTON_ID_KEY_O,
    EDITOR_BUTTON_ID_KEY_P,
    EDITOR_BUTTON_ID_KEY_Q,
    EDITOR_BUTTON_ID_KEY_R,
    EDITOR_BUTTON_ID_KEY_S,
    EDITOR_BUTTON_ID_KEY_T,
    EDITOR_BUTTON_ID_KEY_U,
    EDITOR_BUTTON_ID_KEY_V,
    EDITOR_BUTTON_ID_KEY_W,
    EDITOR_BUTTON_ID_KEY_X,
    EDITOR_BUTTON_ID_KEY_Y,
    EDITOR_BUTTON_ID_KEY_Z,

    // Modifiers
    EDITOR_BUTTON_ID_KEY_LEFT_SHIFT,
    EDITOR_BUTTON_ID_KEY_RIGHT_SHIFT,
    EDITOR_BUTTON_ID_KEY_LEFT_CONTROL,
    EDITOR_BUTTON_ID_KEY_RIGHT_CONTROL,
    EDITOR_BUTTON_ID_KEY_LEFT_ALT,
    EDITOR_BUTTON_ID_KEY_RIGHT_ALT,
    EDITOR_BUTTON_ID_KEY_CAPS_LOCK,

    // Common
    EDITOR_BUTTON_ID_KEY_ESCAPE,
    EDITOR_BUTTON_ID_KEY_BACKSPACE,
    EDITOR_BUTTON_ID_KEY_DELETE,
    EDITOR_BUTTON_ID_KEY_ENTER,
    EDITOR_BUTTON_ID_KEY_TAB,
    EDITOR_BUTTON_ID_KEY_SPACE,

    // Navigation
    EDITOR_BUTTON_ID_KEY_LEFT,
    EDITOR_BUTTON_ID_KEY_RIGHT,
    EDITOR_BUTTON_ID_KEY_UP,
    EDITOR_BUTTON_ID_KEY_DOWN,
    EDITOR_BUTTON_ID_KEY_HOME,
    EDITOR_BUTTON_ID_KEY_END,
    EDITOR_BUTTON_ID_KEY_PAGE_UP,
    EDITOR_BUTTON_ID_KEY_PAGE_DOWN,
    EDITOR_BUTTON_ID_KEY_INSERT,

    // Mouse
    EDITOR_BUTTON_ID_MOUSE_LEFT,
    EDITOR_BUTTON_ID_MOUSE_MIDDLE,
    EDITOR_BUTTON_ID_MOUSE_RIGHT,
    // TODO: what if more mouse buttons?

    EDITOR_BUTTON_ID_COUNT
};

enum EditorInputEventType {
    EDITOR_INPUT_EVENT_BUTTON_DOWN,
    EDITOR_INPUT_EVENT_BUTTON_UP,
    EDITOR_INPUT_EVENT_CODEPOINT,
    EDITOR_INPUT_EVENT_MOUSE_WHEEL,
    EDITOR_INPUT_EVENT_MOUSE_MOVE,
    EDITOR_INPUT_EVENT_COUNT
};

struct EditorInputEvent {
    EditorInputEventType type;
    union {
        struct {
            EditorButtonId button_id;
        };

        struct {
            int codepoint;
        };

        struct {
            float dx, dy;
        } mouse_wheel;

        struct {
            float dx, dy;
        } mouse;
    };
};

#define MAX_INPUT_EVENTS 64

struct EditorInput {
    EditorInputEvent events[MAX_INPUT_EVENTS];
    int event_count;

    f32 mouse_x;
    f32 mouse_y;
};

// TODO: proper allocation of codepoint buffer/on-screen text buffer
#define MAX_CHARS 4096

// TODO: this should not be part of the api -> proper push rendering pipeline for text, ui, etc.
struct EditorState {
    // NOTE: ensure buffer bounds -> most likely alloc on heap later
    // look into how much to keep in buffer, caching, etc.
    int codepoints[MAX_CHARS];
    int codepoint_count;
};

internal void editorUpdate(EditorMemory *memory, EditorInput *input);

#define AUZIO_PLATFORM_H
#endif
