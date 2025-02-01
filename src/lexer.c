#include "lexer.h"

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

// ----- TOKEN ARRAY -----
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

// ----- Lexer -----
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
