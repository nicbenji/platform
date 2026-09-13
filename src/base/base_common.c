internal B32 mem_is_zero(void *ptr, U64 size) {
    U64 extra = (size&0x7); // size % 8
    U64 count8 = (size >> 3); // size / 8

    U64 *p64 = (U64 *)ptr;
    for (U64 i = 0; i < count8; ++i) {
        if (*p64++ != 0) {
          return false;
        }
    }

    U8 *p8 = (U8 *)p64;
    for (U64 i = 0; i < extra; ++i) {
        if (*p8++ != 0) {
          return false;
        }
    }

    return true;
}

internal U16 u16_safe_cast(U32 x) {
    AssertAlways(x <= U16_MAX);
    U16 result = (U16)x;
    return result;
}
