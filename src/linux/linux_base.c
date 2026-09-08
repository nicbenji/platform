#include <sys/mman.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

global_var U64 global_page_size;

internal void *mem_reserve(void *base_addr, U64 size) {
    void *result
        = mmap(base_addr, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED) {
        result = 0;
    }
    return result;
}

internal B32 mem_commit(void *ptr, U64 size) {
    mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return 1;
}

internal void mem_decommit(void *ptr, U64 size) {
    madvise(ptr, size, MADV_DONTNEED);
    mprotect(ptr, size, PROT_NONE);
}

internal void mem_free(void *ptr, U64 size) {
    munmap(ptr, size);
}

internal U64 sys_info_get_page_size(void) {
    return global_page_size;
}

int main(int argc, char **argv) {
    global_page_size = (U64)sysconf(_SC_PAGESIZE);

    main_thread_entrypoint(argc, argv);
}

