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

    token->type = TOK_KEYWORD;

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

typedef struct TokenListNode {
    Token* data;
    struct TokenListNode* next;
} TokenListNode;

typedef struct TokenList {
    int length;
    TokenListNode* head;
} TokenList;

TokenList* token_list_init() {
    TokenList* token_list = malloc(sizeof(TokenList));
    if (!token_list) {
        printf("Failed to allocate memory for token list\n");
        exit(EXIT_FAILURE);
    }

    token_list->length = 0;
    token_list->head = malloc(sizeof(TokenListNode));
    if (!token_list->head) {
        printf("Failed to allocate memory for token list head node\n");
        exit(EXIT_FAILURE);
    }

    token_list->head->data = NULL;
    token_list->head->next = NULL;

    return token_list;
}

void add_token(TokenList* list, Token* token_to_add) {
    if (token_to_add->type == TOK_NONE) {
        printf("Token lexing failed somwhere");
        exit(EXIT_FAILURE);
    }

    if (!list->head->data) {
        list->head->data = token_to_add;
        list->head->next = NULL;
        list->length++;
        return;
    }

    TokenListNode* current_node = list->head;
    while (current_node->next != NULL) {
        current_node = current_node->next;
    }

    current_node->next = malloc(sizeof(TokenListNode));
    if (!current_node->next) {
        printf("Failed to allocate memory for next token in list\n");
        exit(EXIT_FAILURE);
    }

    current_node->next->data = token_to_add;
    current_node->next->next = NULL;
    list->length++;
}

void free_token_list(TokenList* list) {
    TokenListNode* current_node = list->head;
    TokenListNode* previous_node;
    while (current_node != NULL) {
        previous_node = current_node;
        current_node = current_node->next;
        free(previous_node);
    }

    free(current_node);
    free(list);
}

void print_token_list(TokenList* list) {
    printf("List Length: %d\n", list->length);

    TokenListNode* current_token = list->head;
    int i = 1;
    printf("Token %d -> Type: %d, Value: %s, Length: %d\n", i, current_token->data->type, current_token->data->value, current_token->data->length);
    while (current_token->next != NULL) {
        current_token = current_token->next;
        i++;
        printf("Token %d -> Type: %d, Value: %s, Length: %d\n", i, current_token->data->type, current_token->data->value, current_token->data->length);
    }
}

TokenList* lex_src(Lexer* lexer) {
    TokenList* tokens = token_list_init();
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

    return tokens;
}

void parse_tokens(TokenList* tokens) {
    print_token_list(tokens);
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

    TokenList* tokens = lex_src(&lexer);
    parse_tokens(tokens);

    free_token_list(tokens);

    return 0;
}
