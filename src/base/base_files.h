typedef struct {
    U64 u64[1];
} FileHandle;

// TODO: maybe add MakeDirs flag for more convenience in usage
typedef enum {
    FileFlag_Read = (1<<0),
    FileFlag_Write = (1<<1),
    FileFlag_Truncate = (1<<2),
    FileFlag_Append = (1<<3),
    FileFlag_MakeDirs = (1<<4),
} FileFlags;

// NOTE: This api is experimental and subject to huge changes as more use cases pop up

internal Str8 file_read_all(MemoryArena *arena, Str8 file_path);
internal B32 file_replace_all(Str8 file_path, Str8List string_list);
internal B32 file_append(Str8 file_path, Str8 str);

internal FileHandle file_zero(void);
internal B32 file_valid(FileHandle file);

// NOTE: OS-specific impl
internal FileHandle file_open(Str8 file_path, FileFlags flags);
internal void file_close(FileHandle file);
internal U64 file_get_size(FileHandle file);
// TODO: probably more nicely chunked reads -> profiler needed
internal U64 file_read(FileHandle file, U64 start, U64 end, void *buffer);
internal U64 file_write(FileHandle file, U64 start, U64 end, void *buffer);



