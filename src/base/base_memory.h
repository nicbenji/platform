#ifndef BASE_MEMORY_H_
#define BASE_MEMORY_H_

// -nb: OS-specific impl

/**
* The passed adresses need to be aligned to page boundaries.
*/

internal void *mem_reserve(U64 size);
internal B32 mem_commit(void *ptr, U64 size);
internal void mem_decommit(void *ptr, U64 size);
internal void mem_free(void *ptr, U64 size);

#endif  // BASE_MEMORY_H_

