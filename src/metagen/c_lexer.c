global_var Str8 keywords[] = {
    // str8_lit("void"),
    // str8_lit("int"),
    // str8_lit("char"),
    str8_lit("struct"),
    str8_lit("const")
};

internal B32 clex_is_keyword(Str8 identifier) {
    for (U64 i = 0; i < ArrayCount(keywords); ++i) {
        if (str8_equals(keywords[i], identifier)) {
            return true;
        }
    }
    return false;
}

internal B32 clex_is_id_begin(U8 c) {
    B32 result = (char_ascii_is_alpha(c) || c == '_');
    return result;
}

internal B32 clex_is_id_continuation(U8 c) {
    B32 result = (clex_is_id_begin(c) || char_is_digit(c));
    return result;
}

internal Str8 clex_get_id(Str8 str) {
    U8 *at = str.begin;
    U8 *one_past_last = at + str.length;
    while (at < one_past_last && clex_is_id_continuation(*at)) {
        ++at;
    }
    Str8 result = str8_from_range(str.begin, at);
    return result;
}

internal C_TokenKind clex_get_delim_token_kind(U8 c) {
    switch (c) {
        case ';': return TokenKind_Semicolon;
        case ',': return TokenKind_Comma;
        case '(': return TokenKind_OpenParen;
        case ')': return TokenKind_CloseParen;
        case '*': return TokenKind_Star;
        UnreachableDefaultCase; // NOTE: maybe make this a human-readable error msg instead
    }
}

// FIXME: memory allocation! -> dynamic array???
internal C_TokenArray clex(Str8 c_code) {
    C_TokenArray result = {0};
    result.max_count = MAX_TOKENS;

    U8 *lexer_at = c_code.begin;
    U8 *one_past_last = lexer_at + c_code.length;
    while (lexer_at < one_past_last) {
        Str8 to_lex = str8_from_range(lexer_at, one_past_last);
        if (char_is_whitespace(*lexer_at)) {
            ++lexer_at;
        } else if (clex_is_id_begin(*lexer_at)) {
            Token token = {0};
            token.str = clex_get_id(to_lex);
            token.kind = clex_is_keyword(token.str)
                ? TokenKind_Keyword
                : TokenKind_Identifier;

            Assert(result.count < result.max_count);
            result.t[result.count++] = token;
            lexer_at += token.str.length;
        } else {
            Token token = {0};
            token.str = str8(lexer_at, 1);
            token.kind = clex_get_delim_token_kind(*lexer_at);

            Assert(result.count < result.max_count);
            result.t[result.count++] = token;
            ++lexer_at;
        }
    }
    return result;
}
