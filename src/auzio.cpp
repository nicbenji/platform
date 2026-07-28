#include "auzio_platform.h"
#include "auzio_utils.h"

internal void handleButtonDownEvent(EditorState *editor_state, EditorButtonId button_id) {
    switch (button_id) {
        // TODO: ctrl + backspace / ctrl + del semantics
        case EDITOR_BUTTON_ID_KEY_BACKSPACE: {
            --editor_state->codepoint_count;
        } break;
        case EDITOR_BUTTON_ID_KEY_ENTER: {
            editor_state->codepoints[editor_state->codepoint_count++] = '\n';
        } break;
        default: {
            // TODO: logging
        } break;
    }
}

internal void handleButtonUpEvent(EditorState *editor_state, EditorButtonId button_id) {
    switch (button_id) {
        // TODO: ctrl + backspace / ctrl + del semantics
        case EDITOR_BUTTON_ID_KEY_BACKSPACE: {
        } break;
        case EDITOR_BUTTON_ID_KEY_ENTER: {
        } break;
        default: {
            // TODO: logging
        } break;
    }
}

internal void editorUpdate(EditorMemory *memory, EditorInput *input) {
    ASSERT(sizeof(EditorState) <= memory->permanent_memory_size);
    EditorState *editor_state = (EditorState *)memory->permanent_memory;
    if (!memory->is_initialized) {
        editor_state->codepoint_count = 0;

        memory->is_initialized = true;
    }

    for (int event_idx = 0; event_idx < input->event_count; ++event_idx) {
        EditorInputEvent *input_event = &input->events[event_idx];

        switch (input_event->type) {
            case EDITOR_INPUT_EVENT_BUTTON_DOWN: {
                handleButtonDownEvent(editor_state, input_event->button_id);
            } break;

            case EDITOR_INPUT_EVENT_BUTTON_UP: {
                handleButtonUpEvent(editor_state, input_event->button_id);
            } break;

            case EDITOR_INPUT_EVENT_CODEPOINT: {
                ASSERT(editor_state->codepoint_count < MAX_CHARS);
                editor_state->codepoints[editor_state->codepoint_count++]
                    = input_event->codepoint;
            } break;

            case EDITOR_INPUT_EVENT_MOUSE_WHEEL: {

            } break;

            case EDITOR_INPUT_EVENT_MOUSE_MOVE: {

            } break;

            INVALID_DEFAULT_CASE;
        }

    }
}
