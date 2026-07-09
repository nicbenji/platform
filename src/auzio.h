#ifndef EDITORE_H

#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

#define internal static
#define local_persist static
#define global_var static

#if EDITORE_DEBUG
// TODO: complete assertion macro
#define assert(expression) if(!(expression)) {printf("Assertion failed!\n"); *(volatile int *)0 = 0;}
#else
#define assert(expression)
#endif

#define array_count(arr) (sizeof(arr) / sizeof((arr)[0]))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

// TODO: always 64bit??
#define KiB(value) ((value) * 1024)
#define MiB(value) (KiB(value) * 1024)
#define GiB(value) (MiB(value) * 1024)
#define TiB(value) (GiB(value) * 1024)

#define EDITORE_H
#endif
