#ifndef C_LEXER_H_
#define C_LEXER_H_

// TODO: more fine-grained handling of keywords -> void/int/char should be keywords
// separate TokenKind per keyword (or extra KeywordKind)?
// TODO: proper error handling
// TODO: streaming api?? -> lexer_next(Lexer *lexer)

// FIXME: remove this and use "dynamic allocation"
#define MAX_TOKENS 4096

typedef enum {
    TokenKind_OpenParen,
    TokenKind_CloseParen,
    TokenKind_Semicolon,
    TokenKind_Comma,
    TokenKind_Star,
    TokenKind_Keyword,
    TokenKind_Identifier,
} C_TokenKind;

typedef struct {
    C_TokenKind kind;
    Str8 str;
} Token;

// FIXME: make this dynamic arr??
typedef struct {
    Token t[MAX_TOKENS];
    U64 count;
    U64 max_count;
} C_TokenArray;

internal C_TokenArray clex(Str8 c_code);

#endif  // C_LEXER_H_
