#include "base/base_inc.h"
#include "base/base_inc.c"

typedef struct {
    Str8 type;
    Str8 name;
} ParamInfo;

typedef struct {
    Str8 return_type;
    Str8 name;
    ParamInfo *params;
    U64 param_count;
} ProcInfo;

internal int main_entrypoint(int argc, Str8 *argv) {
    MemoryArena *mgen_arena = mem_arena_default();
    Str8 in_file_funcs = file_read_all(mgen_arena, str8_lit("wl_client.signatures"));

    Str8List func_signatures = str8_split(mgen_arena, in_file_funcs, (U8 *)"\n", 1);

    Str8ListNode *signature = func_signatures.first;
    while (signature != 0) {
        printf("%.*s\n", str8_vargs(signature->str));

        signature = signature->next;
    }

    return 0;
}
