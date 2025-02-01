#ifndef Q_LEXER_H
#define Q_LEXER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// ----- TOKEN -----
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

Token* token_init(TokenType type, char* value, int length);

// ----- TOKEN ARRAY -----
typedef struct TokenArray {
    Token** tokens;
    size_t capacity;
    size_t length;
} TokenArray;

TokenArray* token_array_init(size_t capacity);
void add_token(TokenArray* array, Token* token_to_add);
void free_token_array(TokenArray* array);
void print_token_array(TokenArray* array);

// ----- Lexer -----
typedef struct {
    const char* src;
    int src_len;
    char current_char;
    int position;
    int line;
    int column;
} Lexer;

bool isdelim(int chr);

void lexer_init(Lexer* lexer, const char* src);
void lexer_advance(Lexer* lexer);
char lexer_peek_offset(Lexer* lexer, int offset);
void lexer_get_tok_value(Lexer* lexer, Token* token);
Token* lexer_eat_word(Lexer* lexer);
Token* lexer_eat_digit(Lexer* lexer);
Token* lexer_eat_delim(Lexer* lexer);
TokenArray* lex_src(Lexer* lexer);

#endif // Lexer
