internal void wm_event_list_push(
  MemoryArena *arena,
  WM_EventList *events, WM_EventKind kind
) {
    WM_Event *new = mem_arena_push_no_zero(arena, WM_Event, 1);
    new->kind = kind;
    sll_queue_append(events->first, events->last, new);
}
