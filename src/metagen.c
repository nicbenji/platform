#include "base/base_inc.h"
#include "base/base_inc.c"
#include "base/base_strings.h"

#define MAX_TOKENS 4096

typedef enum {
    TokenKind_OpenParen,
    TokenKind_CloseParen,
    TokenKind_Semicolon,
    TokenKind_Comma,
    TokenKind_Star,
    TokenKind_Keyword,
    TokenKind_Identifier,
} TokenKind;

typedef struct {
    TokenKind kind;
    Str8 str;
} Token;

global_var Str8 keywords[] = {
    str8_lit("void"),
    str8_lit("int"),
    str8_lit("struct"),
    str8_lit("const")
};

internal B32 mgen_is_keyword(Str8 identifier) {
    for (U64 i = 0; i < ArrayCount(keywords); ++i) {
        if (str8_equals(keywords[i], identifier)) {
            return true;
        }
    }
    return false;
}

internal B32 mgen_is_identifier_begin(U8 c) {
    B32 result = (char_is_alpha(c) || c == '_');
    return result;
}

internal B32 mgen_is_identifier_continuation(U8 c) {
    B32 result = (mgen_is_identifier_begin(c) || char_is_digit(c));
    return result;
}

internal Str8 mgen_get_identifier(Str8 str) {
    U8 *at = str.begin;
    U8 *one_past_last = at + str.length;
    while (at < one_past_last && mgen_is_identifier_continuation(*at)) {
        ++at;
    }
    Str8 result = str8_from_range(str.begin, at);
    return result;
}

internal TokenKind mgen_get_delim_token_kind(U8 c) {
    switch (c) {
        case ';': return TokenKind_Semicolon;
        case ',': return TokenKind_Comma;
        case '(': return TokenKind_OpenParen;
        case ')': return TokenKind_CloseParen;
        case '*': return TokenKind_Star;
        UnreachableDefaultCase; // NOTE: maybe make this a human-readable error msg instead
    }
}


internal int main_entrypoint(int argc, Str8 *argv) {
    MemoryArena *mgen_arena = mem_arena_default();
    Str8 in_file_funcs = file_read_all(mgen_arena, str8_lit("wl_client.signatures"));
    Assert(in_file_funcs.length > 0);

    // prototype := return_type proc_name '(' parameters ')' ';'
    // parameters := parameter {', ' parameter} | empty | 'void'
    // parameter := type identifier
    // type := base_type { '*' } ;
    // base_type := 'struct' identifier | identifier ;
    // proc_name := identifier
    Token tokens[MAX_TOKENS]; // NOTE: maybe use dynamic array in future
    U64 token_count = 0;

    U8 *at = in_file_funcs.begin;
    U8 *one_past_last = at + in_file_funcs.length;
    while (at < one_past_last) {
        Str8 to_lex = str8_from_range(at, one_past_last);
        if (char_is_whitespace(*at)) {
            ++at;
        } else if (mgen_is_identifier_begin(*at)) {
            Token token = {0};
            token.str = mgen_get_identifier(to_lex);
            token.kind = mgen_is_keyword(token.str)
                ? TokenKind_Keyword
                : TokenKind_Identifier;

            Assert(token_count < 4096);
            tokens[token_count++] = token;
            at += token.str.length;
        } else {
            Token token = {0};
            token.str = str8(at, 1);
            token.kind = mgen_get_delim_token_kind(*at);

            Assert(token_count < 4096);
            tokens[token_count++] = token;
            ++at;
        }
    }

    printf("Tokens:\n");
    for (U64 i = 0; i < token_count; ++i) {
        printf("Kind: %d, Txt: %.*s\n", tokens[i].kind, str8_vargs(tokens[i].str));
    }

    return 0;
}
