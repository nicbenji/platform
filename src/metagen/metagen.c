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
    Str8 result = str8_list_join(arena, &list, 0);
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

internal int main_entrypoint(int argc, Str8 *argv) {
    MemoryArena *mgen_arena = mem_arena_default();
    Str8 in_file_funcs = file_read_all(mgen_arena, str8_lit("wl_client.signatures"));
    Assert(in_file_funcs.length > 0);

    // prototype := return_type proc_name '(' parameters ')' ';'
    // parameters := parameter {', ' parameter} | empty | 'void'
    // parameter := type identifier
    // type := base_type { '*' } ;
    // base_type := 'struct' identifier | identifier ;
    // proc_name := identifier

    // TODO: how should this api allocate?
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

    Str8List proc_bindings_h = {0};
    Str8List proc_bindings_c = {0};

    Str8 h_guard_header = str8_lit("#if !defined(WL_CLIENT_LOADER_H_)\n"
                                   "#define WL_CLIENT_LOADER_H_\n\n");
    str8_list_push(mgen_arena, &proc_bindings_h, h_guard_header);

    Str8 loader_decl
        = str8_lit("internal B32 wl_load_functions(LibraryHandle wl_client_lib, Wl_Functions *wl)");
    str8_list_push(mgen_arena, &proc_bindings_c, loader_decl);
    str8_list_push(mgen_arena, &proc_bindings_c, str8_lit("{\n"));

    for (U64 i = 0; i < proc_count; ++i) {
        Str8 macro_name = str8_to_upper(mgen_arena, procs[i]->name);
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit("#define "));
        str8_list_push(mgen_arena, &proc_bindings_h, macro_name);
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit("(name) "));
        str8_list_push(mgen_arena, &proc_bindings_h, mgen_format_type(mgen_arena, procs[i]->return_type));
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit(" name("));
        for (U64 j = 0; j < procs[i]->params->count; ++j) {
            str8_list_push(mgen_arena, &proc_bindings_h, mgen_format_type(mgen_arena, procs[i]->params->p[j]->type));
            str8_list_push(mgen_arena, &proc_bindings_h, str8_lit(" "));
            str8_list_push(mgen_arena, &proc_bindings_h, procs[i]->params->p[j]->name);
            if (j != (procs[i]->params->count - 1)) {
                str8_list_push(mgen_arena, &proc_bindings_h, str8_lit(", "));
            }
        }
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit(")\n"));

        Str8 proc_type = mgen_proc_name_to_type(mgen_arena, procs[i]->name);
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit("typedef "));
        str8_list_push(mgen_arena, &proc_bindings_h, macro_name);
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit("("));
        str8_list_push(mgen_arena, &proc_bindings_h, proc_type);
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit(");\n\n"));

        Str8 proc_name_no_prefix = mgen_trim_proc_prefix(procs[i]->name);
        str8_list_push(mgen_arena, &proc_bindings_c, str8_lit("    wl->"));
        str8_list_push(mgen_arena, &proc_bindings_c, proc_name_no_prefix);
        str8_list_push(mgen_arena, &proc_bindings_c, str8_lit(" = ("));
        str8_list_push(mgen_arena, &proc_bindings_c, proc_type);
        str8_list_push(mgen_arena, &proc_bindings_c, str8_lit(" *)\n        dynlib_load_proc(wl_client_lib, str8_lit(\""));
        str8_list_push(mgen_arena, &proc_bindings_c, procs[i]->name);
        str8_list_push(mgen_arena, &proc_bindings_c, str8_lit("\"));\n    if(!wl->"));
        str8_list_push(mgen_arena, &proc_bindings_c, proc_name_no_prefix);
        str8_list_push(mgen_arena, &proc_bindings_c, str8_lit(") {\n        return false;\n    }\n\n"));
    }


    str8_list_push(mgen_arena, &proc_bindings_h, str8_lit("typedef struct {\n"));

    for (U64 i = 0; i < proc_count; ++i) {
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit("    "));
        Str8 proc_type = mgen_proc_name_to_type(mgen_arena, procs[i]->name);
        str8_list_push(mgen_arena, &proc_bindings_h, proc_type);
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit(" *"));
        str8_list_push(mgen_arena, &proc_bindings_h, mgen_trim_proc_prefix(procs[i]->name));
        str8_list_push(mgen_arena, &proc_bindings_h, str8_lit(";\n"));
    }

    str8_list_push(mgen_arena, &proc_bindings_c, str8_lit("    return true;\n}"));

    str8_list_push(mgen_arena, &proc_bindings_h, str8_lit("} Wl_Functions;\n\n"));
    str8_list_push(mgen_arena, &proc_bindings_h, loader_decl);
    str8_list_push(mgen_arena, &proc_bindings_h, str8_lit(";\n\n"));

    Str8 h_guard_footer = str8_lit("#endif // WL_CLIENT_LOADER_H_");
    str8_list_push(mgen_arena, &proc_bindings_h, h_guard_footer);

    B32 h_success
        = file_replace_all(str8_lit("../src/linux/generated/wl_client_loader.h"), &proc_bindings_h);
    B32 c_success
        = file_replace_all(str8_lit("../src/linux/generated/wl_client_loader.c"), &proc_bindings_c);
    Assert(h_success && c_success);

    return 0;
}
