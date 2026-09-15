typedef struct {
  U64 u64[1];
} LibraryHandle;

// NOTE: OS-specific impl

internal LibraryHandle dynlib_load(Str8 lib_path);
internal VoidProc *dynlib_load_proc(LibraryHandle lib, Str8 proc_name);
