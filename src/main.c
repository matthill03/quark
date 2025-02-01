#include <complex.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


void print_usage() {
    printf("USAGE: qkc <file_name>\n");
}

typedef enum {
    TOK_NONE = 0,
    TOK_KEYWORD,
    TOK_ID,
    TOK_INT,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_COLON,
    TOK_SEMI,
    TOK_EQUAL,
    TOK_GT,
    TOK_ARROW,
    TOK_DASH,
    TOK_EOF,
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int length;
} Token;

Token* token_init(TokenType type, char* value, int length) {
    Token* token = malloc(sizeof(Token));
    if (!token) {
        printf("Failed to allocate token memory");
        exit(EXIT_FAILURE);
    }

    token->type = type;
    token->value = value;
    token->length = length;

    return token;
}

typedef struct {
    const char* src;
    int src_len;
    char current_char;
    int position;
    int line;
    int column;
} Lexer;

void lexer_init(Lexer* lexer, const char* src) {
    if (src == NULL) { printf("Invalid src for lexer init"); exit(EXIT_FAILURE); }

    lexer->src = src;
    lexer->src_len = strlen(lexer->src);
    lexer->position = 0;
    lexer->current_char = lexer->src[lexer->position];
    lexer->line = 1;
    lexer->column = 1;
}

void lexer_advance(Lexer* lexer) {
    if (lexer == NULL) { printf("Invalid lexer used"); exit(EXIT_FAILURE); }

    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 0;
    } else {
        lexer->column++;
    }

    if (lexer->current_char != '\0' && lexer->position < lexer->src_len) {
        lexer->position++;
        lexer->current_char = lexer->src[lexer->position];
    }

}

char lexer_peek_offset(Lexer* lexer, int offset) {
    return lexer->src[lexer->position + offset];
}

void lexer_get_tok_value(Lexer* lexer, Token* token) {
    token->value = realloc(token->value, (token->length) + 2);

    if (!token->value) {
        printf("Failed to reallocate memory for token\n");
        free(token);
        exit(EXIT_FAILURE);
    }

    token->value[token->length] = lexer->current_char;

    lexer_advance(lexer);
    token->length++;
}

Token* lexer_eat_word(Lexer* lexer) {
    // FIXME: Put this into a better function to include digit lexing.

    Token* token = token_init(TOK_NONE, NULL, 0);
    int start = lexer->position;

    while (isalnum(lexer->current_char) || lexer->current_char == '_') {
        lexer_get_tok_value(lexer, token);
    }
    token->value[token->length] = '\0';

    token->type = TOK_ID;

    return token;
}

Token* lexer_eat_digit(Lexer* lexer) {
    // FIXME: Put this into a better function to include word lexing.

    Token* token = token_init(TOK_NONE, NULL, 0);
    token->type = TOK_INT;

    int start = lexer->position;

    while (isdigit(lexer->current_char) || lexer->current_char == '.') {
        lexer_get_tok_value(lexer, token);
    }
    token->value[token->length] = '\0';

    return token;
}

Token* lexer_eat_delim(Lexer* lexer) {
    switch (lexer->current_char) {
        case ':': lexer_advance(lexer); return token_init(TOK_COLON, ":", 1);
        case '(': lexer_advance(lexer); return token_init(TOK_LPAREN, "(", 1);
        case ')': lexer_advance(lexer); return token_init(TOK_RPAREN, ")", 1);
        case '{': lexer_advance(lexer); return token_init(TOK_LBRACE, "{", 1);
        case '}': lexer_advance(lexer); return token_init(TOK_RBRACE, "}", 1);
        case '=': lexer_advance(lexer); return token_init(TOK_EQUAL, "=", 1);
        case ';': lexer_advance(lexer); return token_init(TOK_SEMI, ";", 1);
        case '-':
            if (lexer_peek_offset(lexer, 1) == '>') {
                lexer_advance(lexer);
                lexer_advance(lexer);

                return token_init(TOK_ARROW, "->", 2);
            }

            return token_init(TOK_DASH, "-", 1);

        case '>': lexer_advance(lexer); return token_init(TOK_GT, ">", 1);
        default: 
            printf("Invalid delim character\n"); 
            lexer_advance(lexer);
            return token_init(TOK_NONE, NULL, 0);
    }
}


bool isdelim(int chr) {
    return (chr == ':' || chr == '(' || chr == ')' || chr == '{' || chr == '}' || chr == '=' || chr == ';' || chr == '-' || chr == '>');
}

typedef struct TokenArray {
    Token** tokens;
    size_t capacity;
    size_t length;
} TokenArray;

TokenArray* token_array_init(size_t capacity) {
    TokenArray* new_array = malloc(sizeof(TokenArray));
    if (!new_array) {
        printf("Failed to allocate memory for token array\n");
        exit(EXIT_FAILURE);
    }

    new_array->capacity = capacity;
    new_array->length = 0;
    new_array->tokens = calloc(new_array->capacity, sizeof(Token*));

    if (!new_array->tokens) {
        printf("Failed to allocate memory for new array tokens\n");
        free(new_array);
        exit(EXIT_FAILURE);
    }

    return new_array;
}

void add_token(TokenArray* array, Token* token_to_add) {
    if (token_to_add->type == TOK_NONE) {
        printf("Token to add is of type 'TOK_NONE', token creation failed somwhere\n");
        exit(EXIT_FAILURE);
    }

    if (array->length >= array->capacity) {
        array->capacity = array->capacity * 2;
        array->tokens = realloc(array->tokens, array->capacity * sizeof(Token));
        if (!array->tokens) {
            printf("Failed to reallocate token array tokens\n");
            exit(EXIT_FAILURE);
        }
    }

    array->tokens[array->length] = token_to_add;
    array->length++;
}

void free_token_array(TokenArray* array) {
    if (!array) {
        printf("There is no array to free\n");
        return;
    }

    free(array->tokens);
    free(array);
}

void print_token_array(TokenArray* array) {
    printf("List Length: %ld\n", array->length);

    for (int i = 0; i < array->length; i++) {
        Token* current_token = array->tokens[i];
        printf("Token %d -> Type: %d, Value: %s, Length: %d\n", i, current_token->type, current_token->value, current_token->length);
     }
}

TokenArray* lex_src(Lexer* lexer) {
    TokenArray* tokens = token_array_init(16);
    while (lexer->current_char != '\0') {
        if (isalpha(lexer->current_char)) {
            add_token(tokens, lexer_eat_word(lexer));
        } else if (isdigit(lexer->current_char)) {
            add_token(tokens, lexer_eat_digit(lexer));
        } else if (isdelim(lexer->current_char)) {
            add_token(tokens, lexer_eat_delim(lexer));
        } else {
            lexer_advance(lexer);
            continue;
        }
    }

    add_token(tokens, token_init(TOK_EOF, NULL, 0));

    return tokens;
}

typedef struct Parser {
    TokenArray* token_array;
    Token* current_token;
    size_t position;
} Parser;

typedef enum {
    AST_PROGRAM,
    AST_FUNCTION_DECL,
    //AST_FUNCTION_CALL,
    AST_VARIABLE_DECL,
    AST_RETURN_STMT,
    AST_LITERAL,
} ASTNodeType;

typedef union {
    struct {
        // This could act as the 'context' for the file??
        // struct ASTNode* varibles;
        struct ASTNode* functions;
    } program;

    struct {
        const char* name;
        const char* return_type;
        struct ASTNode* body;
    } function_decl;

    struct {
        // TODO: Make const a possible thing.
        // bool mut;
        const char* name;
        const char* type;
        struct ASTNode* value;
    } variable_decl;

    struct {
        struct ASTNode* value;
    } return_stmt;

    struct {
        int int_value;
    } literal;
} ASTNodeValue;

typedef struct ASTNode {
    ASTNodeType type;
    ASTNodeValue value;
    struct ASTNode* next;
} ASTNode;

void parse_tokens(Parser* parser, ASTNode* root);
ASTNode* parse_scope(Parser* parser);
ASTNode* parse_decl(Parser* parser);

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


ASTNode* ast_init() {
    ASTNode* new_ast = malloc(sizeof(ASTNode));
    if (!new_ast) {
        printf("Failed to allocate memory for new ast root\n");
        exit(EXIT_FAILURE);
    }

    new_ast->type = AST_PROGRAM;
    new_ast->value.program.functions = calloc(1, sizeof(ASTNode));
    new_ast->next = NULL;

    return new_ast;
}

void ast_append_node(ASTNode* branch_root, ASTNode* node_to_append) {
    if (!branch_root) {
        branch_root = node_to_append;
        return;
    }

    ASTNode* last_node = branch_root;
    while (last_node->next != NULL) {
        last_node = last_node->next;
    }

    last_node->next = node_to_append;
}

ASTNode* ast_create_fn_decl(const char* name, const char* return_type, ASTNode* body) {
    if (!body) {
        printf("Body AST node passed into fn decl is not valid\n");
        exit(EXIT_FAILURE);
    }

    ASTNode* new_fn_decl_node = malloc(sizeof(ASTNode));
    new_fn_decl_node->type = AST_FUNCTION_DECL;

    new_fn_decl_node->value.function_decl.name = name;
    new_fn_decl_node->value.function_decl.return_type = return_type;
    new_fn_decl_node->value.function_decl.body = body;
    new_fn_decl_node->next = NULL;

    return new_fn_decl_node;
}

ASTNode* ast_create_var_decl(const char* name, const char* type, ASTNode* value) {
    if (value->type != AST_LITERAL) {
        printf("Var decl value must be a literal\n");
        exit(EXIT_FAILURE);
    }

    ASTNode* new_var_decl_node = malloc(sizeof(ASTNode));
    if (!new_var_decl_node) {
        printf("Failed to allocate memory for var decl node\n");
        exit(EXIT_FAILURE);
    }

    new_var_decl_node->type = AST_VARIABLE_DECL;
    new_var_decl_node->value.variable_decl.name = name;
    new_var_decl_node->value.variable_decl.type = type;
    new_var_decl_node->value.variable_decl.value = value;
    new_var_decl_node->next = NULL;

    return new_var_decl_node;
}

ASTNode* ast_create_literal(const char* type, int value) {
    ASTNode* new_literal_node = malloc(sizeof(ASTNode));
    if (!new_literal_node) {
        printf("Failed to allocate memory for new literal node\n");
        exit(EXIT_FAILURE);
    }

    new_literal_node->type = AST_LITERAL;
    new_literal_node->value.literal.int_value = value;
    new_literal_node->next = NULL;

    return new_literal_node;
}

ASTNode* ast_create_return_stmt(ASTNode* value) {
    if (!value || value->type != AST_LITERAL) {
        printf("Value given in ret stmt node creation is not valid\n");
        exit(EXIT_FAILURE);
    }

    ASTNode* new_ret_stmt_node = malloc(sizeof(ASTNode));
    if (!new_ret_stmt_node) {
        printf("Failed to allocate memory for new return stmt node\n");
        exit(EXIT_FAILURE);
    }

    new_ret_stmt_node->type = AST_RETURN_STMT;
    new_ret_stmt_node->value.return_stmt.value = value;
    new_ret_stmt_node->next = NULL;

    return new_ret_stmt_node;
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

        parser_advance(parser, TOK_SEMI);
        printf("Returning %s\n", return_value);

        ASTNode* return_value_node = ast_create_literal("i32", atoi(return_value));
        return ast_create_return_stmt(return_value_node);
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
        ASTNode* body = parse_scope(parser);

        printf("Function def: Name -> %s, Return -> %s\n", func_name, return_type);
        return ast_create_fn_decl(func_name, return_type, body);
    }

    const char* name = parser_peek(parser, -1)->value;

    parser_advance(parser, TOK_ID);    // type
    const char* type = parser->current_token->value;

    parser_advance(parser, TOK_EQUAL);      // =

    // TODO: Make this work for any type
    parser_advance(parser, TOK_INT);        // literal
    const char* value = parser->current_token->value;

    parser_advance(parser, TOK_SEMI);

    printf("Var definition: Name -> %s, Type -> %s, Value -> %s\n", name, type, value);
    ASTNode* literal_node = ast_create_literal(type, atoi(value));
    return ast_create_var_decl(name, type, literal_node);
}

ASTNode* parse_scope(Parser* parser) {
    ASTNode* scope_root = calloc(1, sizeof(ASTNode));

    if (parser->current_token->type != TOK_LBRACE) {
        printf("Invalid token found at the start of scope -> %s\n", parser->current_token->value);
        exit(EXIT_FAILURE);
    }

    while (parser_has_tokens(parser) && (parser->current_token->type != TOK_RBRACE)) {
        parse_tokens(parser, scope_root);
    }
    //printf("Token Value: %s, Type: %d\n", parser->current_token->value, parser->current_token->type);

    return scope_root;
}

void parse_tokens(Parser* parser, ASTNode* root) {
    switch (parser->current_token->type) {
        case TOK_ID: {
            ast_append_node(root, parse_id(parser));
            break;
        }
        case TOK_COLON: {
            ast_append_node(root, parse_decl(parser));
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
    //printf("Token Value: %s, Type: %d\n", parser->current_token->value, parser->current_token->type);

    return root;
}

void print_ast(ASTNode* root) {
    if (root->type != AST_PROGRAM) {
        printf("To print the ast, you must pass in the prog root\n");
        return;
    }

    printf("Prog: %p, Type: %d\n", root->value.program.functions, root->type);
    ASTNode* current_node = root->value.program.functions;
    while (current_node->next != NULL) {
        printf("%p, %d", current_node, current_node->type);
        if (current_node->type == AST_FUNCTION_DECL) {
            ASTNode* body = current_node->value.function_decl.body;
            ASTNode* current_body = body;
            while (current_body->next != NULL) {
                switch (current_node->type) {
                    case AST_FUNCTION_DECL:
                        printf("Func Decl -> Name: %s\n", current_node->value.function_decl.name);
                    case AST_VARIABLE_DECL: 
                        printf("Var Decl -> Name: %s\n", current_node->value.variable_decl.name);
                    case AST_RETURN_STMT:
                        printf("Ret Stmt -> Value: %d\n", current_node->value.return_stmt.value->value.literal.int_value);
                    default:
                        printf("%p, %d\n", current_node, current_node->type);
                }
                current_body = current_body->next;
            }
        }
        current_node = current_node->next;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        exit(EXIT_SUCCESS);
    }

    const char* file_path = argv[1];

    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        printf("ERROR: Could not open file -> %s\n", file_path); 
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    const size_t file_size = ftell(file);
    rewind(file);

    char file_content[file_size + 1];

    fread(file_content, file_size, 1, file);
    file_content[file_size] = '\0';
    fclose(file);

    printf("File (%s) size in bytes: %li\n", file_path, file_size);

    Lexer lexer;
    lexer_init(&lexer, file_content);

    TokenArray* tokens = lex_src(&lexer);
    //print_token_array(tokens);

    Parser* parser = parser_init(tokens);
    ASTNode* ast = parse_token_array(parser);
    print_ast(ast);

    free_token_array(tokens);
    free(ast);

    return 0;
}
