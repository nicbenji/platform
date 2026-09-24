#include "../base/base_inc.h"
#include "../base/base_inc.c"
#include "metagen_inc.h"
#include "metagen_inc.c"

internal Str8 mgen_format_type(MemoryArena *arena, C_Type *type) {
    Str8List list = {0};
    if (type->is_const) {
        str8_list_push(arena, &list, str8_lit("const "));
    }
    if (type->is_struct) {
        str8_list_push(arena, &list, str8_lit("struct "));
    }
    str8_list_push(arena, &list, type->name);

    if (type->is_ptr) {
        str8_list_push(arena, &list, str8_lit(" *"));
    }
    Str8 result = str8_list_join(arena, list, 0);
    return result;
}

#define FN_TYPE_POSTFIX str8_lit("Fn")

internal Str8 mgen_proc_name_to_type(MemoryArena *arena, Str8 proc_name) {
    // TODO: add prefix to type! -> pass as arg
    // TODO: capitalisation, casing
    // TODO: simple concat method? or should always go through Str8List? maybe Str8Builder???
    Str8 result = {0};
    result.length = proc_name.length + FN_TYPE_POSTFIX.length;
    result.begin = mem_arena_push_array(arena, U8, result.length);
    MemCopy(result.begin, proc_name.begin, proc_name.length);
    MemCopy(result.begin + proc_name.length, FN_TYPE_POSTFIX.begin, FN_TYPE_POSTFIX.length);
    return result;
}

internal Str8 mgen_trim_proc_prefix(Str8 proc_name) {
    // TODO: Str8 prefix as second arg, then get length and chop that
    Str8 result = str8_chop_left(proc_name, 3);
    return result;
}

// TODO: handle possible write errors here? or return false if error??
internal void mgen_function_loader(
    Str8 file_dir, Str8 lib_prefix,
    C_ProcDeclaration **procs, U64 proc_count
) {
    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);

    Str8 procs_h_file = str8_lit("../src/linux/generated/wl_loader.h");
    Str8 procs_c_file = str8_lit("../src/linux/generated/wl_loader.c");

    file_replace_all(procs_h_file, (Str8List){0}); // TODO: better API for this?
    file_replace_all(procs_c_file, (Str8List){0});
    file_append(procs_h_file, str8_lit("#if !defined(WL_LOADER_H_)\n"
                                       "#define WL_LOADER_H_\n\n"));

    Str8 loader_decl = str8_lit("internal B32 wl_load_functions("
                                "LibraryHandle wl_lib, Wl_Functions *wl)");
    file_append(procs_c_file, loader_decl);
    file_append(procs_c_file, str8_lit("{\n"));

    for (U64 i = 0; i < proc_count; ++i) {
        Str8 macro_name = str8_to_upper(scratch.arena, procs[i]->name);
        file_append(procs_h_file, str8_lit("#define "));
        file_append(procs_h_file, macro_name);
        file_append(procs_h_file, str8_lit("(name) "));
        file_append(procs_h_file, mgen_format_type(scratch.arena, procs[i]->return_type));
        file_append(procs_h_file, str8_lit(" name("));
        for (U64 j = 0; j < procs[i]->params->count; ++j) {
            file_append(procs_h_file, mgen_format_type(scratch.arena, procs[i]->params->p[j]->type));
            file_append(procs_h_file, str8_lit(" "));
            file_append(procs_h_file, procs[i]->params->p[j]->name);
            if (j != (procs[i]->params->count - 1)) {
                file_append(procs_h_file, str8_lit(", "));
            }
        }
        file_append(procs_h_file, str8_lit(")\n"));

        Str8 proc_type = mgen_proc_name_to_type(scratch.arena, procs[i]->name);
        file_append(procs_h_file, str8_lit("typedef "));
        file_append(procs_h_file, macro_name);
        file_append(procs_h_file, str8_lit("("));
        file_append(procs_h_file, proc_type);
        file_append(procs_h_file, str8_lit(");\n\n"));

        Str8 proc_name_no_prefix = mgen_trim_proc_prefix(procs[i]->name);
        file_append(procs_c_file, str8_lit("    wl->"));
        file_append(procs_c_file, proc_name_no_prefix);
        file_append(procs_c_file, str8_lit(" = ("));
        file_append(procs_c_file, proc_type);
        file_append(procs_c_file, str8_lit(" *)\n        dynlib_load_proc(wl_lib, str8_lit(\""));
        file_append(procs_c_file, procs[i]->name);
        file_append(procs_c_file, str8_lit("\"));\n    if(!wl->"));
        file_append(procs_c_file, proc_name_no_prefix);
        file_append(procs_c_file, str8_lit(") {\n        return false;\n    }\n\n"));
    }

    file_append(procs_h_file, str8_lit("typedef struct {\n"));

    for (U64 i = 0; i < proc_count; ++i) {
        file_append(procs_h_file, str8_lit("    "));
        Str8 proc_type = mgen_proc_name_to_type(scratch.arena, procs[i]->name);
        file_append(procs_h_file, proc_type);
        file_append(procs_h_file, str8_lit(" *"));
        file_append(procs_h_file, mgen_trim_proc_prefix(procs[i]->name));
        file_append(procs_h_file, str8_lit(";\n"));
    }

    file_append(procs_c_file, str8_lit("    return true;\n}"));

    file_append(procs_h_file, str8_lit("} Wl_Functions;\n\n"));
    file_append(procs_h_file, loader_decl);
    file_append(procs_h_file, str8_lit(";\n\n"));

    Str8 h_guard_footer = str8_lit("#endif // WL_LOADER_H_");
    file_append(procs_h_file, h_guard_footer);

    thread_ctx_scratch_end(scratch);
}


internal int main_entrypoint(int argc, Str8 *argv) {

#if OS_LINUX
    MemoryArena *mgen_arena = mem_arena_default();
    Str8 in_file_funcs = file_read_all(mgen_arena, str8_lit("wl.signatures"));
    Assert(in_file_funcs.length > 0);

    C_TokenArray tokens = clex(in_file_funcs);
    if (tokens.count == 0) {
        return 0;
    }

    C_ProcDeclaration *procs[MAX_PROCS];
    U64 proc_count = 0;

    C_Parser parser = cparse_begin(mgen_arena, tokens);
    while (!cparse_end(&parser)) {
        C_ProcDeclaration *decl = cparse_declaration(&parser);
        procs[proc_count++] = decl;
    }

    mgen_function_loader(
        str8_lit("../src/linux/generated"), str8_lit("wl"),
        procs, proc_count
    );

#endif

    return 0;
}
