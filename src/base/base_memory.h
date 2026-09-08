#ifndef BASE_MEMORY_H_
#define BASE_MEMORY_H_

// -nb: OS-specific impl

/**
* The passed adresses need to be aligned to page boundaries.
*
* The base_addr in mem_reserve is an optional parameter for debug purposes.
* If NULL is passed it will be ignored.
*/

internal void *mem_reserve(void *base_addr, U64 size);
internal B32 mem_commit(void *ptr, U64 size);
internal void mem_decommit(void *ptr, U64 size);
internal void mem_free(void *ptr, U64 size);

#endif  // BASE_MEMORY_H_

