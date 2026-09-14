#ifndef BASE_STRINGS_H_
#define BASE_STRINGS_H_

/** Basic string and unicode conversion functions
*
*   Take note that string functions taking an arena, add a NULL Terminator ('\0')
*   to the end of the string. This makes it very easy to call other APIs.
*/

typedef struct {
    U8 *begin;
    U64 length;
} Str8;

typedef struct {
    U16 *begin;
    U64 length;
} Str16;

internal Str8 str8(U8 *begin, U64 length);
internal Str8 str8_from_cstr(const char *cstr);
internal Str16 str16(U16 *wstr, U64 length);

internal Str8 str8_copy(MemoryArena *arena, Str8 utf8_str);

internal Str8 str8_from_str16(MemoryArena *arena, Str16 utf16_str);
internal Str16 str16_from_str8(MemoryArena *arena, Str8 utf8_str);

typedef struct {
    U32 codepoint;
    U32 size;
} UnicodeDecoder;

#define UNICODE_INVALID_CODEPOINT 0

// Decode the given U8*/U16* into a unicode codepoint if the codepoint size is
// smaller than the specified max.
// Returns U32_MAX as a sentinal value on invalid input.
internal UnicodeDecoder utf8_decode(U8 *str, U64 max);
internal UnicodeDecoder utf16_decode(U16 *str, U64 max);

// Writes the UTF-8/UTF-16 encoded codepoint into the passed destination buffer.
// Make sure the dst buffer is large enough.
// Returns the amount of U8s/U16s used or 0/UNICODE_INVALID_CODEPOINT on invalid codepoints.
internal U32 utf8_encode(U8 *dst, U32 codepoint);
internal U32 utf16_encode(U16 *dst, U32 codepoint);

#define str8_literal(cstr_lit) str8((U8 *)cstr_lit, sizeof(cstr_lit) - 1)
#define str8_make_printable(s) (int)((s).length), ((s).begin)

#endif  // BASE_STRINGS_H_
