typedef struct {
  U64 u64[1];
} LibraryHandle;

// NOTE: OS-specific impl

internal LibraryHandle dynlib_load(const char *lib);
internal VoidProc *dynlib_load_proc(LibraryHandle lib, const char *symbol);
