#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void print_usage() {
    printf("USAGE: qkc <file_name>\n");
}

typedef enum {
    TOK_NONE = 0,
    TOK_ID,
    TOK_INT,
    TOK_COLON,
    TOK_SEMI,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_EQUAL,
    TOK_DASH,
    TOK_GT,
} TokenType;

char* tok_type_as_string(TokenType type) {
    switch (type) {
        case TOK_NONE: return "TOK_NONE";
        case TOK_ID: return "TOK_ID";
        case TOK_INT: return "TOK_INT";
        case TOK_COLON: return "TOK_COLON";
        case TOK_SEMI: return "TOK_SEMI";
        case TOK_LPAREN: return "TOK_LPAREN";
        case TOK_RPAREN: return "TOK_RPAREN";
        case TOK_LBRACE: return "TOK_LBRACE";
        case TOK_RBRACE: return "TOK_RBRACE";
        case TOK_EQUAL: return "TOK_EQUAL";
        case TOK_DASH: return "TOK_DASH";
        case TOK_GT: return "TOK_GT";
        default: return "INVALID";
    };
}

typedef struct {
    TokenType type;
    int start;
    int length;
} Token;

bool isdelim(int chr) {
    return (chr == ':' || chr == '(' || chr == ')' || chr == '{' || chr == '}' || chr == '=' || chr == ';' || chr == '-' || chr == '>');
}

void parse_content(char* file_content) {
    int start = 0, end = 0, length = 0;

    int i = 0;
    while (file_content[i] != '\0') {
        if (isalpha(file_content[i])) {
            start = i;
            while (!isspace(file_content[i]) && !isdelim(file_content[i])) {
                end = i;
                i++;
            }
            length = (end - start) + 1;
            printf("ALPHA: Token -> %s, Value -> %.*s, Length -> %d\n", tok_type_as_string(TOK_ID), length, file_content + start, length);
        } else if (isdigit(file_content[i])) {
            start = i;
            while (!isspace(file_content[i]) && !isdelim(file_content[i])) {
                end = i;
                i++;
            }
            length = (end - start) + 1;
            printf("DIGIT: Token -> %s, Value -> %.*s, Length -> %d\n", tok_type_as_string(TOK_INT), length, file_content + start, length);
        } else if (isdelim(file_content[i])) {
            i++;
        } else if (isspace(file_content[i])) {
            i++;
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        exit(EXIT_SUCCESS);
    }

    const char* file_path = argv[1];

    long file_size = 0;

    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        printf("ERROR: Could not open file -> %s\n", file_path); 
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);

    fseek(file, 0, SEEK_SET);
    char file_content[file_size + 1];

    fread(file_content, file_size, 1, file);
    file_content[file_size] = '\0';
    fclose(file);

    printf("File (%s) size in bytes: %li\n", file_path, file_size);
    //printf("%s\n", file_content);

    parse_content(file_content);



    return 0;
}
