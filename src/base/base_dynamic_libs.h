typedef struct {
  U64 u64[1];
} Library;

// NOTE: OS-specific impl

internal Library dynlib_load(Str8 lib_path);
internal VoidProc *dynlib_load_proc(Library lib, Str8 proc_name);
