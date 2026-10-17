#include "../base/base_inc.h"
#include "../base/base_inc.c"
#include "metagen_inc.h"
#include "metagen_inc.c"

internal Str8 mgen_format_type(MemoryArena *arena, C_Type *type) {
    Str8List list = str8_list_zero();
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

#define FN_TYPE_POSTFIX str8_lit("_fn")

internal Str8 mgen_proc_name_to_type(MemoryArena *arena, Str8 proc_name) {
    // TODO: add prefix to type! -> pass as arg
    // TODO: capitalisation, casing
    // TODO: simple concat method? or should always go through Str8List? maybe Str8Builder???
    Str8 result = str8_zero();
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
internal void mgen_function_loader(Str8 file_dir, Str8 proc_defs_path) {
    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);

    Str8 in_file_funcs = file_read_all(scratch.arena, proc_defs_path);
    Assert(in_file_funcs.length > 0); // TODO: error?

    C_TokenArray tokens = clex(in_file_funcs);
    if (tokens.count == 0) {
        return;
    }

    C_ProcDeclaration *procs[MAX_PROCS];
    U64 proc_count = 0;

    C_Parser parser = cparse_begin(scratch.arena, tokens);
    for (; !cparse_end(&parser);) {
        C_ProcDeclaration *decl = cparse_declaration(&parser);
        procs[proc_count++] = decl;
    }

    U8 *one_past_last_prefix = proc_defs_path.begin + (proc_defs_path.length - 1);
    for (; *one_past_last_prefix != '.';) {
        --one_past_last_prefix;
    }

    Str8 lib_prefix = str8_ascii_to_upper(
        scratch.arena,
        str8_from_range(proc_defs_path.begin, one_past_last_prefix)
    );
    Str8 lower_lib_prefix = str8_ascii_to_lower(scratch.arena, lib_prefix);

    Str8List file_parts = str8_list_zero();
    str8_list_push(scratch.arena, &file_parts, file_dir);
    if (file_dir.begin[file_dir.length - 1] != '/') {
        str8_list_push(scratch.arena, &file_parts, str8_lit("/"));
    }

    str8_list_push(scratch.arena, &file_parts, lower_lib_prefix);
    str8_list_push(scratch.arena, &file_parts, str8_lit("_loader.h"));

    Str8 procs_h_file = str8_list_join(scratch.arena, file_parts, 0);
    Str8 procs_c_file = str8_copy(scratch.arena, procs_h_file);
    procs_c_file.begin[procs_c_file.length - 1] = 'c';

    file_replace_all(procs_h_file, str8_list_zero());
    file_replace_all(procs_c_file, str8_list_zero());
    file_append(procs_h_file, str8_lit("#if !defined("));
    file_append(procs_h_file, lib_prefix);
    file_append(procs_h_file, str8_lit("_LOADER_H_)\n"));
    file_append(procs_h_file, str8_lit("#define "));
    file_append(procs_h_file, lib_prefix);
    file_append(procs_h_file, str8_lit("_LOADER_H_\n\n"));

    Str8List loader_decl_parts = str8_list_zero();
    str8_list_push(scratch.arena, &loader_decl_parts, str8_lit("internal B32 "));
    str8_list_push(scratch.arena, &loader_decl_parts, lower_lib_prefix);
    str8_list_push(scratch.arena, &loader_decl_parts,
                   str8_lit("_load_functions(Library lib, "));
    str8_list_push(scratch.arena, &loader_decl_parts, lib_prefix);
    str8_list_push(scratch.arena, &loader_decl_parts, str8_lit("_Functions *fns)"));

    Str8 loader_decl = str8_list_join(scratch.arena, loader_decl_parts, 0);

    file_append(procs_c_file, loader_decl);
    file_append(procs_c_file, str8_lit("{\n"));

    for (U64 i = 0; i < proc_count; ++i) {
        Str8 macro_name = str8_ascii_to_upper(scratch.arena, procs[i]->name);
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
        file_append(procs_c_file, str8_lit("    fns->"));
        file_append(procs_c_file, proc_name_no_prefix);
        file_append(procs_c_file, str8_lit(" = ("));
        file_append(procs_c_file, proc_type);
        file_append(procs_c_file, str8_lit(" *)\n        dynlib_load_proc(lib, str8_lit(\""));
        file_append(procs_c_file, procs[i]->name);
        file_append(procs_c_file, str8_lit("\"));\n    if(!fns->"));
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

    file_append(procs_h_file, str8_lit("} "));
    file_append(procs_h_file, lib_prefix);
    file_append(procs_h_file, str8_lit("_Functions;\n\n"));
    file_append(procs_h_file, loader_decl);
    file_append(procs_h_file, str8_lit(";\n\n"));

    file_append(procs_h_file, str8_lit("#endif // "));
    file_append(procs_h_file, lib_prefix);
    file_append(procs_h_file, str8_lit("_LOADER_H_"));

    thread_ctx_scratch_end(scratch);
}


internal int main_entrypoint(int argc, Str8 *argv) {

#if OS_LINUX
    mgen_function_loader(
        str8_lit("../src/linux/generated"),
        str8_lit("wl.signatures")
    );
#endif

    return 0;
}
