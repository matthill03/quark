#include "parser.h"
#include "ast.h"

// ----- PARSER -----
Parser* parser_init(TokenArray* token_array) {
    Parser* new_parser = malloc(sizeof(Parser));
    if (!new_parser) {
        printf("Failed to allocate memory for new parser\n");
        exit(EXIT_FAILURE);
    }

    new_parser->token_array = token_array;
    new_parser->position = 0;
    new_parser->current_token = new_parser->token_array->tokens[new_parser->position];

    return new_parser;
}

void parser_advance(Parser* parser, TokenType expected_type) {
    if (parser->position + 1 >= parser->token_array->length) {
        printf("Parser has reached the final token\n");
        return;
    }

    Token* next_token = parser->token_array->tokens[parser->position + 1];
    if ((next_token->type != expected_type) && expected_type != TOK_NONE) {
        printf("Token %s, does not have type %d\n", next_token->value, expected_type);
        exit(EXIT_FAILURE);
    }

    if (parser->position >= parser->token_array->length) {
        printf("Parser has reached the end of the token list\n");
        return;
    }

    if (expected_type == TOK_ARROW) {
        parser->position += 2;
        parser->current_token = parser->token_array->tokens[parser->position];
        return;
    }

    parser->position++;
    parser->current_token = parser->token_array->tokens[parser->position];
}

Token* parser_peek(Parser* parser, int offset) {
    return parser->token_array->tokens[parser->position + offset];
}

int parser_has_tokens(Parser* parser) {
    return (parser->position < parser->token_array->length && parser->current_token->type != TOK_EOF);
}

ASTNode* parse_id(Parser* parser) {
    if (strcmp(parser->current_token->value, "return") == 0) {
        // TODO: Make return stuff proper...
        //  |-- Check if in a function scope (valid return).
        //  |-- Then see if the return value is of same return type. 'return;' is of return type NONE
        //  |-- Then grab return value and use it.
        parser_advance(parser, TOK_INT);
        const char* return_value = parser->current_token->value;

        ASTNode* return_stmt_node = ast_create_return_stmt(ast_create_literal("i32", atoi(return_value)));

        parser_advance(parser, TOK_SEMI);

        return return_stmt_node;
    }

    parser_advance(parser, 0);
    return NULL;
}

ASTNode* parse_decl(Parser* parser) {
    if (strcmp(parser_peek(parser, 1)->value, "fn") == 0) {
        const char* func_name = parser_peek(parser, -1)->value; // main

        parser_advance(parser, TOK_ID);         // func
        parser_advance(parser, TOK_LPAREN);     // (

        // TODO: Parse parameters

        parser_advance(parser, TOK_RPAREN);     // )
        parser_advance(parser, TOK_ARROW);      // ->

        const char* return_type = parser->current_token->value;

        parser_advance(parser, TOK_LBRACE);      // {

        // ASTNode* body = parse_scope(parser);
        ASTNode* body = ast_create_empty();
        parse_scope(parser, body);

        ASTNode* ast_func_node = ast_create_fn_decl(func_name, return_type, body);
        return ast_func_node;
    }

    const char* name = parser_peek(parser, -1)->value;

    parser_advance(parser, TOK_ID);    // type
    const char* type = parser->current_token->value;

    parser_advance(parser, TOK_EQUAL);      // =

    // TODO: Make this work for any type
    parser_advance(parser, TOK_INT);        // literal
    const char* value = parser->current_token->value;

    parser_advance(parser, TOK_SEMI);

    ASTNode* ast_var_node = ast_create_var_decl(name, type, ast_create_literal(type, atoi(value)));
    return ast_var_node;
}

void parse_scope(Parser* parser, ASTNode* body) {
    if (parser->current_token->type != TOK_LBRACE) {
        printf("Invalid token found at the start of scope -> %s\n", parser->current_token->value);
        exit(EXIT_FAILURE);
    }

    while (parser_has_tokens(parser) && (parser->current_token->type != TOK_RBRACE)) {
        parse_tokens(parser, body);
    }
}

void parse_tokens(Parser* parser, ASTNode* root) {
    if (!root) {
        printf("Current token -> %s\n", parser->current_token->value);
    }

    switch (parser->current_token->type) {
        case TOK_ID: {
            ASTNode* new_node = parse_id(parser);
            if (!new_node) break;

            ast_append_node(&root, new_node);
            break;
        }
        case TOK_COLON: {
            ast_append_node(&root, parse_decl(parser));
            break;
        }
        default:
            parser_advance(parser, 0);

    }
}


ASTNode* parse_token_array(Parser* parser) {
    ASTNode* root = ast_init();
    while (parser_has_tokens(parser)) {
        parse_tokens(parser, root->value.program.functions);
    }
    return root;
}
