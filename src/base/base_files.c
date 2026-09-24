internal Str8 file_read_all(MemoryArena *arena, Str8 file_path) {
    Str8 result = {0};
    FileHandle file = file_open(file_path, FileAccessFlag_Read);

    if (!file_valid(file)) {
        return result;
    }

    U64 file_size = file_get_size(file);
    result.begin = mem_arena_push_array(arena, U8, file_size);
    result.length = file_size;

    // TODO: proper buffering
    U64 bytes_read = file_read(file, 0, file_size, result.begin);
    if (bytes_read < file_size) {
        mem_arena_pop(arena, file_size - bytes_read);
        result.length = bytes_read;
    }

    file_close(file);
    return result;
}

// TODO: what to do when node_count == 0
internal B32 file_replace_all(Str8 file_path, Str8List string_list) {
    B32 result = false;

    FileHandle file = file_open(file_path,
        FileAccessFlag_Write|FileAccessFlag_Truncate|FileAccessFlag_MakeDirs);
    if (!file_valid(file)) return result;

    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);

    // TODO: proper buffering
    Str8 to_write = str8_list_join(scratch.arena, string_list, 0);
    U64 write_result = file_write(file, 0, to_write.length, to_write.begin);

    result = (write_result == string_list.total_length);
    if (!result) {
        // TODO: error
        Todo; // proper buffering + generally more robust file read/write
    }

    file_close(file);
    thread_ctx_scratch_end(scratch);
    return result;
}

// TODO: what to do when str.length == 0
internal B32 file_append(Str8 file_path, Str8 str) {
    B32 result = false;
    FileHandle file = file_open(file_path,
        FileAccessFlag_Write|FileAccessFlag_Append|FileAccessFlag_MakeDirs);
    if (!file_valid(file)) return result;

    U64 file_size = file_get_size(file);
    U64 bytes_written = file_write(file, file_size, file_size + str.length, str.begin);
    result = (bytes_written == str.length);

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
