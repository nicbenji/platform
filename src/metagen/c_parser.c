internal B32 cparse_expect(C_Parser *parser, C_TokenKind expected) {
    B32 result = (parser->at->kind == expected);
    if (result) {
        ++parser->at;
    }
    return result;
}

internal Str8 cparse_id(C_Parser *parser) {
    Str8 result = {0};
    if (parser->at->kind == TokenKind_Identifier) {
        result = parser->at->str;
        ++parser->at;
    }
    return result;
}

internal C_Type *cparse_type(C_Parser *parser) {
    C_Type *result = mem_arena_push_array(parser->arena, C_Type, 1);
    if (parser->at->kind == TokenKind_Keyword && str8_equals(parser->at->str, str8_lit("const"))) {
        result->is_const = true;
        ++parser->at;
    }

    if (parser->at->kind == TokenKind_Keyword && str8_equals(parser->at->str, str8_lit("struct"))) {
        result->is_struct = true;
        ++parser->at;
    }

    result->name = cparse_id(parser);
    if (!result->name.begin) {
        // TODO: error
        return 0;
    }

    if (parser->at->kind == TokenKind_Star) {
        result->is_ptr = true;
        ++parser->at;
    }

    return result;
}

internal C_ParamArray *cparse_params(C_Parser *parser) {
    C_ParamArray *result = mem_arena_push_array_zero(parser->arena, C_ParamArray, 1);

    B32 only_void = parser->at->kind == TokenKind_Identifier
        && str8_equals(parser->at->str, str8_lit("void"))
        && (parser->at + 1)->kind == TokenKind_CloseParen;
    if (only_void) {
        ++parser->at;
        return result;
    }

    while (parser->at->kind != TokenKind_CloseParen) {
        C_Param *curr_param = mem_arena_push_array(parser->arena, C_Param, 1);

        curr_param->type = cparse_type(parser);
        if (!curr_param->type) {
            // TODO: error
            return 0;
        }

        curr_param->name = cparse_id(parser);
        if (!curr_param->name.begin) {
            // TODO: error
            return 0;
        }

        result->p[result->count++] = curr_param;

        if (parser->at->kind == TokenKind_CloseParen) {
            break;
        } else if (!cparse_expect(parser, TokenKind_Comma)) {
            // TODO: error
            return 0;
        }
    }

    return result;
}

internal C_Parser cparse_begin(MemoryArena *arena, C_TokenArray tokens) {
    C_Parser result = {0};
    result.arena = arena;
    result.at = tokens.t;
    result.end = result.at + tokens.count;
    return result;
}

internal B32 cparse_end(C_Parser *parser) {
    B32 result = (parser->at >= parser->end);
    return result;
}

internal C_ProcDeclaration *cparse_declaration(C_Parser *parser) {
    C_ProcDeclaration *result
        = mem_arena_push_array(parser->arena, C_ProcDeclaration, 1);

    result->return_type = cparse_type(parser);
    if (!result->return_type) {
        // TODO: error
        return 0;
    }

    result->name = cparse_id(parser);
    if (!result->name.begin) {
        // TODO: error
        return 0;
    }


    if (!cparse_expect(parser, TokenKind_OpenParen)) {
        // TODO: error
        return 0;
    }

    result->params = cparse_params(parser);
    if (!result->params) {
        // TODO: error
        return 0;
    }


    if (!cparse_expect(parser, TokenKind_CloseParen)) {
        // TODO: error
        return 0;
    }

    if (!cparse_expect(parser, TokenKind_Semicolon)) {
        // TODO: error
        return 0;
    }
    return result;
}
