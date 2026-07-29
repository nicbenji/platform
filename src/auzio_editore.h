#include "auzio_platform.h"
#ifndef AUZIO_EDITORE_H
#include "auzio_utils.h"

struct AE_Cursor {
    // TODO: check how many lines a 10GB file might have -> maybe need u64
    u32 line, col;
    u32 width, height;
};

struct AE_EditField {
    AP_Rect bounds;
    b32 active;
};

// TODO: feel of the app -> what to do on close misclicks? (add some margin?)
inline b32 ae_rectContainsPoint(AP_Rect rect, V2 point) {
    b32 result = (
        point.x >= rect.top_left.x
        && point.x <= rect.bottom_right.x
        && point.y >= rect.top_left.y
        && point.y <= rect.bottom_right.y
    );
    return result;
}

// TODO: proper allocation of codepoint buffer/on-screen text buffer
#define MAX_CHARS 4096

// FIXME: namespacing -> differnt prefixes between here and platform.h???
// TODO: this should not be part of the api -> proper push rendering pipeline for text, ui, etc.
struct AE_State {
    // NOTE: ensure buffer bounds -> most likely alloc on heap later
    // look into how much to keep in buffer, caching, etc.
    int codepoints[MAX_CHARS];
    int codepoint_count;

    AE_Cursor edit_cursor;
    AE_EditField edit_field;
};

#define AUZIO_EDITORE_H
#endif
