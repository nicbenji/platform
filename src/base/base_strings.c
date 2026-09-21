/* Constructors */

internal Str8 str8(U8 *begin, U64 length) {
    Str8 result = { begin, length };
    return result;
}

internal Str8 str8_from_cstr(const char *cstr) {
    U64 length = strlen(cstr);
    Str8 result = str8((U8 *)cstr, length);
    return result;
}

internal Str8 str8_from_range(U8 *begin, U8 *one_past_last) {
    U64 length = (U64)(one_past_last - begin);
    Str8 result = str8(begin, length);
    return result;
}

internal Str16 str16(U16 *begin, U64 length) {
    Str16 result = { begin, length };
    return result;
}

/* Str8 ops */

internal Str8 str8_copy(MemoryArena *arena, Str8 utf8_str) {
    Str8 result;
    result.length = utf8_str.length;
    result.begin = mem_arena_push_array(arena, U8, result.length + 1);
    MemCopy(result.begin, utf8_str.begin, result.length);
    result.begin[result.length] = '\0';
    return result;
}

internal B32 str8_equals(Str8 a, Str8 b) {
    if (a.length != b.length) {
        return false;
    }

    // TODO: think about SIMD mem-compares?
    B32 result = MemEquals(a.begin, b.begin, a.length);
    return result;
}

internal Str8List str8_split(
    MemoryArena *arena, Str8 str,
    U8* split_chars, U64 split_char_count
) {
    Str8List result = {0};

    U8 *one_past_last = str.begin + str.length;
    for (U8 *at = str.begin; at < one_past_last; ++at) {
        U8 *first_char = at;
        for (; at < one_past_last; ++at) {
            B32 is_split_char = false;
            for (U64 i = 0; i < split_char_count; ++i) {
                if (*at == split_chars[i]) {
                    is_split_char = true;
                    break;
                }
            }

            if (is_split_char) {
                break;
            }
        }

        Str8 split_str = str8_from_range(first_char, at);
        if (split_str.length > 0) {
            str8_list_push(arena, &result, split_str);
        }
    }

    return result;
}

internal Str8 str8_chop_left(Str8 str, U64 amount) {
    U64 clamped_amount = ClampTop(str.length, amount);
    Str8 result = str8(str.begin + clamped_amount, str.length - clamped_amount);
    return result;
}

internal Str8 str8_to_lower(MemoryArena *arena, Str8 str) {
    Str8 result = str8_copy(arena, str);
    for (U64 i = 0; i < str.length; ++i) {
        result.begin[i] = char_ascii_to_lower(str.begin[i]);
    }
    return result;
}

internal Str8 str8_to_upper(MemoryArena *arena, Str8 str) {
    Str8 result = str8_copy(arena, str);
    for (U64 i = 0; i < str.length; ++i) {
        result.begin[i] = char_ascii_to_upper(str.begin[i]);
    }
    return result;
}

/* Conversion */

internal Str8 str8_from_str16(MemoryArena *arena, Str16 utf16_str) {
    Str8 result = {0};
    if (utf16_str.length == 0) return result;

    U64 alloc_size = utf16_str.length * 3;
    U8 *utf8_str = mem_arena_push_array(arena, U8, alloc_size + 1); // +1 for '\0'

    U16 *at = utf16_str.begin;
    U16 *one_past_last = at + utf16_str.length;

    UnicodeDecoder decoded;
    for (; at < one_past_last; at += decoded.size) {
        decoded = utf16_decode(at, (U64)(one_past_last - at));
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

    U8 *at = utf8_str.begin;
    U8 *one_past_last = at + utf8_str.length;

    UnicodeDecoder decoded;
    for (; at < one_past_last; at += decoded.size) {
        decoded = utf8_decode(at, (U64)(one_past_last - at));
        result.length += utf16_encode(utf16_str + result.length, decoded.codepoint);
    }
    utf16_str[result.length] = '\0';

    mem_arena_pop(arena, (alloc_size - result.length) * 2);

    result.begin = utf16_str;
    return result;
}

/* Unicode */

#define UTF8_CONT_BITS_AMT 6

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
        U32 high_surrogate = (str[0] - 0xd800u) << 10;
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

/* Str8List ops */

internal Str8ListNode *str8_list_push(MemoryArena *arena, Str8List *list, Str8 str) {
    Str8ListNode *new = mem_arena_push_array(arena, Str8ListNode, 1);
    sll_queue_push_back(list->first, list->last, new);
    ++list->node_count;
    list->total_length += str.length;
    new->str = str;
    return new;
}

internal Str8 str8_list_join(MemoryArena *arena, Str8List *list, StringJoiner *optional_args) {
    StringJoiner joiner = {0};
    if (optional_args != 0) {
        MemCopyStruct(&joiner, optional_args);
    }
    U64 separator_count = 0;
    if (list->node_count > 0) {
        separator_count = list->node_count - 1;
    }

    Str8 result;
    result.length = list->total_length
        + joiner.prefix.length
        + joiner.postfix.length
        + (separator_count * joiner.separator.length);
    result.begin = mem_arena_push_array(arena, U8, result.length + 1);

    U8 *concat_at = result.begin;
    MemCopy(concat_at, joiner.prefix.begin, joiner.prefix.length);
    concat_at += joiner.prefix.length;
    for (Str8ListNode *curr = list->first; curr != 0; curr = curr->next) {
        MemCopy(concat_at, curr->str.begin, curr->str.length);
        concat_at += curr->str.length;

        if (curr->next != 0) {
            MemCopy(concat_at, joiner.separator.begin, joiner.separator.length);
            concat_at += joiner.separator.length;
        }
    }
    MemCopy(concat_at, joiner.postfix.begin, joiner.postfix.length);
    concat_at += joiner.postfix.length;
    *concat_at = '\0';

    return result;
}

/* Char helpers */
internal B32 char_is_whitespace(U8 c) {
    B32 result = (c == ' ' || c == '\r' || c == '\n'
        || c == '\t' || c == '\v' || c == '\f');
    return result;
}

internal B32 char_ascii_is_lower(U8 c) {
    B32 result = (c >= 'a' && c <= 'z');
    return result;
}

internal B32 char_ascii_is_upper(U8 c) {
    B32 result = (c >= 'A' && c <= 'Z');
    return result;
}

internal B32 char_ascii_is_alpha(U8 c) {
    B32 result = (char_ascii_is_upper(c) || char_ascii_is_lower(c));
    return result;
}

internal B32 char_is_digit(U8 c) {
    B32 result = (c >= '0' && c <= '9');
    return result;
}

internal U8 char_ascii_to_lower(U8 c) {
    if (char_ascii_is_upper(c)) {
        c += (U8)('a' - 'A');
    }
    return c;
}

internal U8 char_ascii_to_upper(U8 c) {
    if (char_ascii_is_lower(c)) {
        c += (U8)('A' - 'a');
    }
    return c;
}



