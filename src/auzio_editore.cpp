#include "auzio_platform.h"
#include "auzio_utils.h"
#include "auzio_editore.h"


internal void ap_editorUpdateAndRender(
    AP_Memory *memory,
    AP_Input *input,
    AP_RenderBuffer *render_buffer,
    AP_Rect screen_rect
) {
    ASSERT(sizeof(AE_State) <= memory->permanent_memory_size);
    AE_State *editor_state = (AE_State *)memory->permanent_memory;
    if (!memory->is_initialized) {
        editor_state->codepoint_count = 0;
        render_buffer->renderable_count = 0;

        memory->is_initialized = true;
    }

    editor_state->edit_field.bounds = screen_rect;

    for (int event_idx = 0; event_idx < input->event_count; ++event_idx) {
        AP_InputEvent *input_event = &input->events[event_idx];

        switch (input_event->type) {
            case AP_INPUT_EVENT_BUTTON_DOWN: {
                switch (input_event->button_id) {
                    // TODO: ctrl + backspace / ctrl + del semantics
                    case AP_BUTTON_ID_KEY_BACKSPACE: {
                        if (!editor_state->edit_field.active) break;
                        --editor_state->codepoint_count;
                    } break;
                    case AP_BUTTON_ID_KEY_ENTER: {
                        if (!editor_state->edit_field.active) break;
                        editor_state->codepoints[editor_state->codepoint_count++] = '\n';
                    } break;
                    case AP_BUTTON_ID_KEY_DOWN: {
                    } break;
                    case AP_BUTTON_ID_KEY_LEFT: {
                    } break;
                    case AP_BUTTON_ID_KEY_UP: {
                    } break;
                    case AP_BUTTON_ID_KEY_RIGHT: {
                    } break;

                    case AP_BUTTON_ID_MOUSE_LEFT: {
                        if (ae_rectContainsPoint(editor_state->edit_field.bounds, input->mouse_pos)) {
                            editor_state->edit_field.active = true;

                            AE_Cursor *edit_cursor = &editor_state->edit_cursor;
                            // NOTE: maybe work with a cursor_index first instead of col + line
                            edit_cursor->col = 0;
                            edit_cursor->line = 0;
                            edit_cursor->height = 40;
                            edit_cursor->width = 4;
                        }
                    } break;

                    default: {
                        // TODO: logging
                    } break;
                }
            } break;

            case AP_INPUT_EVENT_BUTTON_UP: {
                switch (input_event->button_id) {
                    // TODO: ctrl + backspace / ctrl + del semantics
                    case AP_BUTTON_ID_KEY_BACKSPACE: {
                    } break;
                    case AP_BUTTON_ID_KEY_ENTER: {
                    } break;
                    default: {
                        // TODO: logging
                    } break;
                }
            } break;

            case AP_INPUT_EVENT_CODEPOINT: {
                if (!editor_state->edit_field.active) break;

                ASSERT(editor_state->codepoint_count < MAX_CHARS);
                editor_state->codepoints[editor_state->codepoint_count++]
                    = input_event->codepoint;
            } break;

            INVALID_DEFAULT_CASE;
        }

    }

    AP_Renderable clear_renderable = {};
    clear_renderable.type = AP_RENDERABLE_TYPE_CLEAR;
    clear_renderable.bg_color = ap_createColor(0, 0, 0, 255);
    render_buffer->renderables[render_buffer->renderable_count++] = clear_renderable;

    AP_Renderable codepoints_renderable = {};
    codepoints_renderable.type = AP_RENDERABLE_TYPE_CODEPOINTS;
    codepoints_renderable.codepoints = editor_state->codepoints;
    codepoints_renderable.codepoint_count = editor_state->codepoint_count;
    codepoints_renderable.font_color = ap_createColor(255, 255, 255, 255);
    codepoints_renderable.font_handle = 1; // TODO:
    codepoints_renderable.font_size = 40;
    codepoints_renderable.spacing = .0f;
    codepoints_renderable.pos = createV2(5, 5);
    render_buffer->renderables[render_buffer->renderable_count++] = codepoints_renderable;

}
