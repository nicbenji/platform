#include "base/base_inc.h"
#include "base/base_inc.c"

#define MAX_TOKENS 4096
#define MAX_PROCS 1024
#define MAX_PARAMS 128

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

typedef struct {
    // NOTE: name for now -> lets see what we actually need
    Str8 name;
    B32 is_ptr;
} Type;

typedef struct {
    Type *type;
    Str8 name;
} Param;

typedef struct {
    Param params[MAX_PARAMS];
    U64 param_count;
} ParamArray;

typedef struct {
    Type *return_type;
    Str8 name;
    ParamArray *params;
} ProcSignature;

typedef struct {
    MemoryArena *arena;
    Token *at;
} Parser;


global_var Str8 keywords[] = {
    // str8_lit("void"),
    // str8_lit("int"),
    // str8_lit("char"),
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
    B32 result = (ascii_char_is_alpha(c) || c == '_');
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


internal B32 mgen_parser_at(Parser *parser, TokenKind to_check) {
    B32 result = (parser->at->kind == to_check);
    return result;
}


internal B32 mgen_parser_expect(Parser *parser, TokenKind expected) {
    B32 result = mgen_parser_at(parser, expected);
    if (result) {
        ++parser->at;
    }
    return result;
}

internal Type *mgen_parse_type(Parser *parser) {
    Type *result = 0;
    Str8List type_parts = {0};
    // FIXME: verify exact combination of keywords!!
    if (mgen_parser_at(parser, TokenKind_Keyword)) {
        str8_list_push(parser->arena, &type_parts, parser->at->str);
        ++parser->at;
    }
    if (mgen_parser_at(parser, TokenKind_Keyword)) {
        str8_list_push(parser->arena, &type_parts, parser->at->str);
        ++parser->at;
    }

    if (mgen_parser_at(parser, TokenKind_Identifier)) {
        str8_list_push(parser->arena, &type_parts, parser->at->str);
        ++parser->at;
    } else {
        // TODO: error
        return result;
    }

    result = mem_arena_push_array(parser->arena, Type, 1);
    if (mgen_parser_at(parser, TokenKind_Star)) {
        result->is_ptr = true;
        ++parser->at;
    }

    StringJoiner joiner = {0};
    joiner.separator = str8_lit(" ");
    result->name = str8_list_join(parser->arena, &type_parts, &joiner);
    return result;
}

internal Str8 mgen_parse_identifier(Parser *parser) {
    Str8 result = {0};
    if (mgen_parser_at(parser, TokenKind_Identifier)) {
        result = parser->at->str;
        ++parser->at;
    }
    return result;
}

internal ParamArray *mgen_parse_parameters(Parser *parser) {
    ParamArray *result = mem_arena_push_array_zero(parser->arena, ParamArray, 1);
    if (mgen_parser_at(parser, TokenKind_CloseParen)) {
        // no params
        return result;
    }
    if (mgen_parser_at(parser, TokenKind_Keyword) && str8_equals(parser->at->str, str8_lit("void"))) {
        // no params
        ++parser->at;
        return result;
    }

    while (true) {
        Type *type = mgen_parse_type(parser);
        if (type == 0) {
            // TODO: error
            return 0;
        }

        Str8 id = mgen_parse_identifier(parser);
        if (id.begin == 0) {
            // TODO: error
            return 0;
        }

        result->params[result->param_count].type = type;
        result->params[result->param_count].name = id;
        ++result->param_count;

        if (mgen_parser_at(parser, TokenKind_CloseParen)) {
            break;
        } else if (!mgen_parser_expect(parser, TokenKind_Comma)) {
            // TODO: error
            return 0;
        }
    }
    return result;
}


internal ProcSignature *mgen_parse_declaration(Parser *parser) {
    ProcSignature *result = 0;
    Type *return_type = mgen_parse_type(parser);
    if (return_type == 0) {
        // TODO: error
        return result;
    }
    Str8 name = mgen_parse_identifier(parser);
    if (name.begin == 0) {
        // TODO: error
        return result;
    }
    if (!mgen_parser_expect(parser, TokenKind_OpenParen)) {
        // TODO: error
        return result;
    }

    ParamArray *params = mgen_parse_parameters(parser);
    if (params == 0) {
        // TODO: error
        return result;
    }
    if (!mgen_parser_expect(parser, TokenKind_CloseParen)) {
        // TODO: error
        return result;
    }
    if (!mgen_parser_expect(parser, TokenKind_Semicolon)) {
        // TODO: error
        return result;
    }

    result = mem_arena_push_array(parser->arena, ProcSignature, 1);
    result->return_type = return_type;
    result->params = params;
    result->name = name;
    return result;
}


// TODO: replace fixed-size array usages with arena/scratch arena
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

    U8 *lexer_at = in_file_funcs.begin;
    U8 *one_past_last = lexer_at + in_file_funcs.length;
    while (lexer_at < one_past_last) {
        Str8 to_lex = str8_from_range(lexer_at, one_past_last);
        if (char_is_whitespace(*lexer_at)) {
            ++lexer_at;
        } else if (mgen_is_identifier_begin(*lexer_at)) {
            Token token = {0};
            token.str = mgen_get_identifier(to_lex);
            token.kind = mgen_is_keyword(token.str)
                ? TokenKind_Keyword
                : TokenKind_Identifier;

            Assert(token_count < 4095);
            tokens[token_count++] = token;
            lexer_at += token.str.length;
        } else {
            Token token = {0};
            token.str = str8(lexer_at, 1);
            token.kind = mgen_get_delim_token_kind(*lexer_at);

            Assert(token_count < 4095);
            tokens[token_count++] = token;
            ++lexer_at;
        }
    }

    if (token_count == 0) {
        return 0;
    }

    Parser parser = {0};
    parser.at = &tokens[0];
    parser.arena = mgen_arena;

    // FIXME: rework allocation strategy for parser -> when to use arena, when ptrs, when stack??
    ProcSignature procs[MAX_PROCS];
    U64 proc_count = 0;

    Token *one_past_last_token = &tokens[token_count];
    while (parser.at < one_past_last_token) {
        ProcSignature *proc_info = mgen_parse_declaration(&parser);
        Assert(proc_info != 0);
        procs[proc_count++] = *proc_info;
    }

    return 0;
}
