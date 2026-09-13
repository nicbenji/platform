#if !defined(BASE_COMMON_H_)
#define BASE_COMMON_H_

// TODO: add read_only, thread and intrinsic stuff?

/* Basic types */
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;

typedef S8 B8;
typedef S16 B16;
typedef S32 B32;
typedef S64 B64;

#define true 1
#define false 0

typedef float F32;
typedef double F64;

/* Static aliases */
#define internal static
#define local_persist static
#define global_var static


/* Linkage */
#if LANGUAGE_CPP
# define C_LINKAGE_BEGIN extern "C" {
# define C_LINKAGE_END }
# define C_LINKAGE extern "C"
#else
# define C_LINKAGE_BEGIN
# define C_LINKAGE_END
# define C_LINKAGE
#endif

/* Alignment */
#if COMPILER_MSVC
#  define AlignOf(T) __alignof(T)
#elif COMPILER_CLANG
#  define AlignOf(T) __alignof(T)
#elif COMPILER_GCC
#  define AlignOf(T) __alignof__(T)
#else
#  error AlignOf needs to be defined for this compiler
#endif

#if COMPILER_MSVC
#  define MAX_ALIGN_T double
#elif COMPILER_CLANG || COMPILER_GCC
#  include <stddef.h>
#  define MAX_ALIGN_T max_align_t
#else
#  error Missing MAX_ALIGN_T for this compiler.
#endif


#define AlignUpPow2(x, align) ((x) + ((align) - 1) & (~((align) - 1)))

/* Memory ops */

internal B32 mem_is_zero(void *ptr, U64 size);

#include <string.h>
#define MemCopy(dst, src, size) memmove((dst), (src), (size))
#define MemSet(dst, byte, size) memset((dst), (byte), (size))
#define MemCompare(a, b, size) memcmp((a), (b), (size))

#define MemCopyStruct(dst, src) MemCopy((dst), (src), sizeof(*(dst)))
#define MemCopyArray(dst, src) MemCopy((dst), (src), sizeof(dst))
#define MemCopyTyped(dst, src, count) MemCopy((dst), (src), sizeof(*(dst)) * (count))

#define MemZero(src, count) MemSet((src), 0, (count))
#define MemZeroStruct(src) MemZero((src), sizeof(*(src)))
#define MemZeroArray(arr) MemZero((arr), sizeof(arr))
#define MemZeroTyped(typed_ptr, count) MemZero(typed_ptr), sizeof(*(typed_ptr)) * (count))

#define MemMatch(a, b, z) (MemCompare((a), (b), (z)) == 0)
#define MemMatchStruct(a, b) MemMatch((a), (b), sizeof(*(a)))
#define MemMatchArray(a, b) MemMatch((a), (b), sizeof(a))

#define MemIsZeroStruct(ptr) mem_is_zero(ptr, sizeof(*(ptr)))

/* Assert */

#if COMPILER_MSVC
# define Trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
# define Trap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

#define AssertAlways(x) do{if(!(x)) {Trap();}}while(0)
#if BUILD_DEBUG
# define Assert(x) AssertAlways(x)
#else
# define Assert(x) (void)(x)
#endif

#define StaticAssert(condition, ID) global_var U8 Glue(ID, __LINE__)[(condition) ? 1 : -1]

#define Unreachable Assert(!"Invalid code path")
#define UnreachableDefaultCase default: { Unreachable; } break
#define Todo Assert(!"Todo")
#define NoOp ((void)0)

/* Simple helper macros */
#define ArrayCount(arr) (sizeof(arr) / sizeof((arr)[0]))
#define Swap(T,a,b) do{T t__ = a; a = b; b = t__;}while(0)

#define Min(x, y) ((x) < (y) ? (x) : (y))
#define Max(x, y) ((x) > (y) ? (x) : (y))
#define Clamp(a, x, b) (((x) < (a) ? (a) : ((x) > (b) ? (b) : (x))))
#define ClampBot(x, y) Max(x, y)
#define ClampTop(x, y) Min(x, y)

#define Stringify_(s) #s
#define Stringify(s) Stringify_(s)
#define Glue_(x, y) x##y
#define Glue(x, y) Glue_(x, y)

#if ARCH_X64
#  define IntFromPtr(ptr) ((U64)(ptr))
#elif ARCH_X86
#  define IntFromPtr(ptr) ((U32)(ptr))
#else
#  error Missing pointer-to-integer cast for this architecture.
#endif
#define PtrFromInt(i) (void*)(i)

#define IsPow2OrZero(x) (((x) & ((x) - 1)) == 0)
#define IsPow2(x) (((x) != 0) && IsPow2OrZero(x))

#define Pow2(x) (1 << (x))

/* Units */
#define KiB(value) ((value) * 1024)
#define MiB(value) (KiB(value) * 1024)
#define GiB(value) (MiB(value) * 1024)
#define TiB(value) (GiB(value) * 1024)

/* Misc */

// OS-specific impl
internal U64 sys_info_get_page_size(void);

/* Type conversions */

internal U16 u16_safe_cast(U32 x);

/* Constants */

#define SIGN32 ((U32)0x80000000u)
#define EXPONENT32 ((U32)0x7F800000u)
#define MANTISSA32 ((U32)0x007FFFFFu)

#define BIG_GOLDEN32 ((F32)1.61803398875f)
#define SMALL_GOLDEN32 ((F32)0.61803398875f)

#define PI32 ((F32)3.1415926535897f)

#define MACHINE_EPSILON64 ((F64)4.94065645841247e-324)

#define U64_MAX ((U64)0xffffffffffffffffull)
#define U32_MAX ((U32)0xffffffffu)
#define U16_MAX ((U16)0xffffu)
#define U8_MAX ((U8)0xffu)

#define S64_MAX ((S64)0x7fffffffffffffffll)
#define S32_MAX ((S32)0x7fffffff)
#define S16_MAX ((S16)0x7fff)
#define S8_MAX ((S8)0x7f)

#define S64_MIN ((S64)-0x7fffffffffffffffll - 1)
#define S32_MIN ((S32)-0x7fffffff - 1)
#define S16_MIN ((S16)-0x7fff - 1)
#define S8_MIN ((S8)-0x7f - 1)


internal inline F32
positive_inf32(void) {
    union { U32 u; F32 f; } x;
    x.u = EXPONENT32;
    return(x.f);
}

internal inline F32
negative_inf32(void) {
    union { U32 u; F32 f; } x;
    x.u = SIGN32 | EXPONENT32;
    return(x.f);
}

#define Bitmask(x) ((1ull << (x)) - 1)
#define Bit(x) (1ull << ((x) - 1))

#endif // BASE_COMMON_H_
