#ifndef Q_PARSER_H
#define Q_PARSER_H
#include "lexer.h"
#include "ast.h"

// ----- PARSER -----
typedef struct Parser {
    TokenArray* token_array;
    Token* current_token;
    size_t position;
} Parser;

Parser* parser_init(TokenArray* token_array);
void parser_advance(Parser* parser, TokenType expected_type);
Token* parser_peek(Parser* parser, int offset);

int parser_has_tokens(Parser* parser);

ASTNode* parse_id(Parser* parser);
ASTNode* parse_decl(Parser* parser);
void parse_scope(Parser* parser, ASTNode* body);
void parse_tokens(Parser* parser, ASTNode* root);
ASTNode* parse_token_array(Parser* parser);

#endif
