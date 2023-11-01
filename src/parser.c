#include "include/parser.h"
#include <stdio.h>
#include <string.h>

parser_t* init_parser(lexer_t* lexer) {
    parser_t* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);

    return parser;
}

void parser_eat(parser_t* parser, int token_type) {
    if (parser->current_token->type == token_type) {
        parser->current_token = lexer_get_next_token(parser->lexer);
    }
    else {
        printf("Unexpected Token `%s`, with type `%d`", parser->current_token->value, parser->current_token->type);
        exit(1);
    }
}

AST_t* parser_parse(parser_t* parser) {
  return parser_parse_statements(parser);
}

AST_t* parser_parse_statement(parser_t* parser) {
    switch(parser->current_token->type) {
        case TOKEN_ID: return parser_parse_id(parser); 
    }
}

AST_t* parser_parse_statements(parser_t* parser) {

    AST_t* compound = init_ast(AST_COMPOUND);
    compound->compound_value = calloc(1, sizeof(struct AST_t*));

    AST_t* ast_statement = parser_parse_statement(parser);
    compound->compound_value[0] = ast_statement;
    compound->compound_size += 1;

    while(parser->current_token->type == TOKEN_SEMI) {
        parser_eat(parser, TOKEN_SEMI);

        AST_t* ast_statement = parser_parse_statement(parser);
        compound->compound_size += 1;
        compound->compound_value = realloc(compound->compound_value, compound->compound_size * sizeof(struct AST_STRUCT*));
        compound->compound_value[compound->compound_size - 1] = ast_statement;
    }

    return compound;
}

AST_t* parser_parse_expr(parser_t* parser) {}

AST_t* parser_parse_factor(parser_t* parser) {}

AST_t* parser_parse_term(parser_t* parser) {}

AST_t* parser_parse_function_call(parser_t* parser) {}

AST_t* parser_parse_id(parser_t* parser) {
    if(strcmp(parser->current_token->value, "var") == 0) {
        return parser_parse_variable_definition(parser);
    }
    else {
        return parser_parse_variable(parser);
    }
}

AST_t* parser_parse_variable(parser_t* parser) {

    char* token_value = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);

    if(parser->current_token->type == TOKEN_LPAREN) {
        return parser_parse_function_call(parser);
    }

    AST_t* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    return ast_variable;
}

AST_t* parser_parse_variable_definition(parser_t* parser) {
  parser_eat(parser, TOKEN_ID); // var
  char* variable_definition_variable_name = parser->current_token->value;
  parser_eat(parser, TOKEN_ID); // var name
  parser_eat(parser, TOKEN_EQUALS);
  AST_t* variable_definition_value = parser_parse_expr(parser);

  AST_t* variable_definition = init_ast(AST_VARIABLE_DEFINITION);
  variable_definition->variable_definition_variable_name = variable_definition_variable_name;
  variable_definition->variable_definition_value = variable_definition_value;

  return variable_definition;
}

AST_t* parser_parse_string(parser_t* parser) {}

