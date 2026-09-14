#include "base/base_inc.h"
#include "base/base_inc.c"

#include <stdio.h>

#define EvalPrint(x) printf("%s = %d\n", #x, (int)x)

internal void test_ctx_cracking(void) {
    EvalPrint(COMPILER_CLANG);
    EvalPrint(COMPILER_GCC);
    EvalPrint(COMPILER_MSVC);

    EvalPrint(OS_LINUX);
    EvalPrint(OS_WINDOWS);
    EvalPrint(OS_MAC);

    EvalPrint(ARCH_X64);
    EvalPrint(ARCH_X86);
    EvalPrint(ARCH_ARM32);
    EvalPrint(ARCH_ARM64);

    EvalPrint(LANGUAGE_CPP);
    EvalPrint(LANGUAGE_C);
}

typedef struct {
    S32 bar;
    U64 baz;
} Foo;

internal void test_arena(void) {
    MemoryArena *arena = mem_arena_default();

    Assert(arena->pos == MEM_ARENA_HEADER_SIZE);
    Assert(arena->reserved == MiB(64));
    Assert(arena->committed == KiB(64));

    Foo *foo = mem_arena_push_struct(arena, Foo);

    EvalPrint(foo->bar);
    EvalPrint(arena->pos);
    Assert(arena->pos == AlignUpPow2(MEM_ARENA_HEADER_SIZE, MEM_ARENA_DEFAULT_ALIGN) + sizeof(Foo));

    U64 prev_pos = arena->pos;
    MemArena_Temp tmp = mem_arena_temp_begin(arena);

    Foo *foos = mem_arena_push_array_zero(tmp.arena, Foo, 4100);
    EvalPrint(arena->pos);
    EvalPrint(arena->committed);
    Assert(arena->pos <= arena->committed);

    for (int i = 0; i < 4100; ++i) {
        Assert(mem_is_zero(foos, 4100 * sizeof(Foo)));
    }

    mem_arena_temp_end(tmp);
    Assert(arena->pos == prev_pos);

    mem_arena_clear(arena);
    Assert(arena->pos == MEM_ARENA_HEADER_SIZE);

    mem_arena_free(arena);
}

internal void test_common(void) {
    Assert(AlignOf(Foo) == 8);
    Assert(AlignUpPow2(23, 16) == 32);

    MemoryArena *arena = mem_arena_default();
    Foo *foo1 = mem_arena_push_struct(arena, Foo);
    foo1->bar = 69;
    foo1->baz = 420;

    Foo *foo2 = mem_arena_push_struct(arena, Foo);
    MemCopyStruct(foo2, foo1);
    Assert(MemMatchStruct(foo1, foo2));

    foo2->bar = 1;
    foo2->baz = 1;

    Swap(Foo *, foo1, foo2);
    Assert(foo1->bar == 1);
    Assert(foo1->baz == 1);

    Foo foos[] = { *foo1, *foo2 };
    Assert(ArrayCount(foos) == 2);

    MemZeroStruct(foo2);
    MemIsZeroStruct(foo2);

    mem_arena_free(arena);

    printf("%s = %ld\n", "S64_MIN", S64_MIN);
    EvalPrint(S32_MIN);
    EvalPrint(S16_MIN);
    EvalPrint(S8_MIN);
}

internal void test_strings(void) {

}

internal void test_unicode(MemoryArena *arena) {

}

internal int main_entrypoint(int argc, Str8 *argv) {
    test_ctx_cracking();
    test_arena();
    test_common();

    MemoryArena *arena = mem_arena_default();
    test_strings();
    test_unicode(arena);

    return 0;
}
