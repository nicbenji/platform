#include "base_common.h"
internal Str8 str8(U8 *begin, U64 length) {
    Str8 result = { begin, length };
    return result;
}

internal Str8 str8_from_cstr(const char *cstr) {
    U64 length = strlen(cstr);
    Str8 result = str8((U8 *)cstr, length);
    return result;
}

internal Str16 str16(U16 *wstr, U64 length) {
    Str16 result = { wstr, length };
    return result;
}

internal Str8 str8_from_str16(MemoryArena *arena, Str16 utf16_str) {
    Str8 result = {0};
    if (utf16_str.length == 0) return result;

    U64 alloc_size = utf16_str.length * 3;
    U8 *utf8_str = mem_arena_push_array(arena, U8, alloc_size + 1); // +1 for '\0'

    U16 *ptr = utf16_str.begin;
    U16 *one_past_last = ptr + utf16_str.length;

    UnicodeDecoder decoded;
    for (; ptr < one_past_last; ptr += decoded.size) {
        decoded = utf16_decode(ptr, one_past_last - ptr);
        result.length += utf8_encode(utf8_str + result.length, decoded.codepoint);
    }
    utf8_str[result.length] = '\0';

    mem_arena_pop(arena, alloc_size - result.length);

    result.begin = utf8_str;
    return result;
}

internal Str16 str16_from_str8(MemoryArena *arena, Str8 utf8_str) {
    Str16 result = {0};
    if (utf8_str.length == 0) return result;

    U64 alloc_size = utf8_str.length * 2;
    U16 *utf16_str = mem_arena_push_array(arena, U16, alloc_size + 1); // +1 for '\0'

    U8 *ptr = utf8_str.begin;
    U8 *one_past_last = ptr + utf8_str.length;

    UnicodeDecoder decoded;
    for (; ptr < one_past_last; ptr += decoded.size) {
        decoded = utf8_decode(ptr, one_past_last - ptr);
        result.length += utf16_encode(utf16_str + result.length, decoded.codepoint);
    }
    utf16_str[result.length] = '\0';

    mem_arena_pop(arena, (alloc_size - result.length) * 2);

    result.begin = utf16_str;
    return result;
}

#define UTF8_CONT_BITS_AMT 6
#define UNICODE_INVALID_CODEPOINT 0

global_var U8 utf8_class[32] = {
      1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,
      0,0,0,0,0,0,0,0,
      2,2,2,2,3,3,4,5,
};

internal UnicodeDecoder utf8_decode(U8 *str, U64 max) {
    UnicodeDecoder result = { U32_MAX, 1 };
    // TODO: some more special case handling might be needed
    // TODO: All macro or method for readable predicates

    U8 byte = str[0];
    U8 byte_class = utf8_class[byte >> 3];

    if (byte_class == 0 || max < (byte_class - 1)) {
        return result;
    }

    result.size = byte_class;
    switch (byte_class) {
        case 1: {
            result.codepoint = byte;
        } break;
        case 2: {
            U8 continuation_byte = str[1];
            if (utf8_class[continuation_byte >> 3] == 0) {
                result.codepoint = (byte & Bitmask(5)) << UTF8_CONT_BITS_AMT;
                result.codepoint |= (continuation_byte & Bitmask(UTF8_CONT_BITS_AMT));
            }
        } break;
        case 3: {
            U8 continuation_byte[] = { str[1], str[2] };
            if (
                utf8_class[continuation_byte[0] >> 3] == 0
                && utf8_class[continuation_byte[1] >> 3] == 0
            ) {
                result.codepoint = (byte & Bitmask(4)) << (UTF8_CONT_BITS_AMT*2);
                result.codepoint |= (continuation_byte[0] & Bitmask(UTF8_CONT_BITS_AMT)) << UTF8_CONT_BITS_AMT;
                result.codepoint |= (continuation_byte[1] & Bitmask(UTF8_CONT_BITS_AMT));
            }
        } break;
        case 4: {
            U8 continuation_byte[] = { str[1], str[2], str[3] };
            if (
                utf8_class[continuation_byte[0] >> 3] == 0
                && utf8_class[continuation_byte[1] >> 3] == 0
                && utf8_class[continuation_byte[2] >> 3] == 0
            ) {
                result.codepoint = (byte & Bitmask(3)) << (UTF8_CONT_BITS_AMT*3);
                result.codepoint |= (continuation_byte[0] & Bitmask(UTF8_CONT_BITS_AMT)) << (UTF8_CONT_BITS_AMT*2);
                result.codepoint |= (continuation_byte[1] & Bitmask(UTF8_CONT_BITS_AMT)) << UTF8_CONT_BITS_AMT;
                result.codepoint |= (continuation_byte[2] & Bitmask(UTF8_CONT_BITS_AMT));
            }
        } break;
        default: {
            result.size = 1;
        } break;
    }

    return result;
}

internal UnicodeDecoder utf16_decode(U16 *str, U64 max) {
    UnicodeDecoder result = { str[0], 1 };

    B32 is_surrogate_pair = max > 1
        && str[0] >= 0xd800 && str[0] <= 0xdbff
        && str[1] >= 0xdc00 && str[1] <= 0xdfff;
    if (is_surrogate_pair) {
        U32 high_surrogate = (str[0] - 0xd800) << 10;
        U32 low_surrogate = str[1] - 0xdc00;
        result.codepoint = (high_surrogate | low_surrogate) + 0x10000;
        result.size = 2;
    }
    return result;
}

internal U32 utf8_encode(U8 *dst, U32 codepoint) {
    U32 codepoint_size = UNICODE_INVALID_CODEPOINT;

    if (codepoint <= 0x7f) {
        dst[0] = (U8)codepoint;
        codepoint_size = 1;
    } else if (codepoint <= 0x7ff) {
        dst[0] = (Bitmask(2) << 6) | ((codepoint >> UTF8_CONT_BITS_AMT) & Bitmask(5));
        dst[1] = Bit(8) | (codepoint & Bitmask(6));
        codepoint_size = 2;
    } else if (codepoint <= 0xffff && (codepoint < 0xd800 || codepoint > 0xdfff)) {
    // TODO: should really filter out surrogates??? -> probs not
        dst[0] = (Bitmask(3) << 5) | ((codepoint >> (UTF8_CONT_BITS_AMT*2)) & Bitmask(4));
        dst[1] = Bit(8) | ((codepoint >> UTF8_CONT_BITS_AMT) & Bitmask(UTF8_CONT_BITS_AMT));
        dst[2] = Bit(8) | (codepoint & Bitmask(UTF8_CONT_BITS_AMT));
        codepoint_size = 3;
    } else if (codepoint <= 0x10ffff) {
        dst[0] = (Bitmask(4) << 4) | ((codepoint >> (UTF8_CONT_BITS_AMT*3)) & Bitmask(3));
        dst[1] = Bit(8) | ((codepoint >> (UTF8_CONT_BITS_AMT*2)) & Bitmask(UTF8_CONT_BITS_AMT));
        dst[2] = Bit(8) | ((codepoint >> (UTF8_CONT_BITS_AMT)) & Bitmask(UTF8_CONT_BITS_AMT));
        dst[3] = Bit(8) | (codepoint & Bitmask(UTF8_CONT_BITS_AMT));
        codepoint_size = 4;
    }

    return codepoint_size;
}

internal U32 utf16_encode(U16 *dst, U32 codepoint) {
    U32 codepoint_size = UNICODE_INVALID_CODEPOINT;

    // TODO: should really filter out surrogates??? -> probs not
    if (codepoint <= 0xd7ff || (codepoint >= 0xe000 && codepoint < 0xffff)) {
        dst[0] = (U16)codepoint;
        codepoint_size = 1;
    } else if (codepoint >= 0x10000 && codepoint <= 0x10ffff) {
        U32 surrogate_pair = codepoint - 0x10000;
        dst[0] = u16_safe_cast(0xd800 + (surrogate_pair >> 10));
        dst[1] = u16_safe_cast(0xdc00 + (surrogate_pair & Bitmask(10)));
        codepoint_size = 2;
    }

    return codepoint_size;
}

