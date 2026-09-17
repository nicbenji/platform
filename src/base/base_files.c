internal Str8 file_read_all(MemoryArena *arena, Str8 file_path) {
    Str8 result = {0};
    FileHandle file = file_open(file_path, FileAccessFlag_Read);

    if (!file_valid(file)) {
        return result;
    }

    U64 file_size = file_get_size(file);
    result.begin = mem_arena_push_array(arena, U8, file_size);
    result.length = file_size;

    U64 bytes_read = file_read(file, 0, file_size, result.begin);
    if (bytes_read < file_size) {
        mem_arena_pop(arena, file_size - bytes_read);
        result.length = bytes_read;
    }

    file_close(file);
    return result;
}

internal FileHandle file_zero(void) {
    FileHandle result = {0};
    return result;
}

internal B32 file_valid(FileHandle file) {
    FileHandle zero_handle = file_zero();
    B32 result = !MemEqualsStruct(&file, &zero_handle);
    return result;
}
