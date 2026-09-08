#ifndef BASE_STRINGS_H_
#define BASE_STRINGS_H_

typedef struct {
    U8 *begin;
    U64 length;
} Str8;

#define str8_make_printable(s) (int)((s).length), ((s).begin)


#endif  // BASE_STRINGS_H_
