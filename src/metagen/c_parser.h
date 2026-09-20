#ifndef C_PARSER_H_
#define C_PARSER_H_

// FIXME: make these obsolete/unused
#define MAX_PROCS 1024
#define MAX_PARAMS 128

typedef struct {
    Token *begin;
    Token *end;
} C_TokenRange;

typedef struct {
    MemoryArena *arena;
    Token *at;
    Token *end; // one past the end, do not deref
} C_Parser;

typedef struct {
    Str8 name;
    B32 is_ptr;
    B32 is_const;
    B32 is_struct;
} C_Type;

typedef struct {
    C_Type *type;
    Str8 name;
} C_Param;

// TODO: make this dynamic array?
typedef struct {
    C_Param *p[MAX_PARAMS];
    U64 count;
} C_ParamArray;

typedef struct {
    C_Type *return_type;
    Str8 name;
    C_ParamArray *params;
} C_ProcDeclaration;

internal C_Parser cparse_begin(MemoryArena *arena, C_TokenArray tokens);
internal B32 cparse_end(C_Parser *parser);
internal C_ProcDeclaration *cparse_declaration(C_Parser *parser);

#endif  // C_PARSER_H_
