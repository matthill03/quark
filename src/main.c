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
    TOK_ARROW,
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
        case TOK_ARROW: return "TOK_ARROW";
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

void parse_content(const char* file_content) {
    int i = 0;
    while (file_content[i] != '\0') {
        int start = i;
        int length = 0;

        if (isspace(file_content[i])) {
            i++;
            continue;
        }

        if (isalpha(file_content[i])) {
            while (isalnum(file_content[i]) || file_content[i] == '_') {
                i++;
            }
            length = i - start;
            printf("ALPHA: Token -> %s, Value -> %.*s, Length -> %d\n", tok_type_as_string(TOK_ID), length, file_content + start, length);
        } else if (isdigit(file_content[i])) {
            while (isdigit(file_content[i])) {
                i++;
            }
            length = i - start;
            printf("DIGIT: Token -> %s, Value -> %.*s, Length -> %d\n", tok_type_as_string(TOK_INT), length, file_content + start, length);
        } else if (isdelim(file_content[i])) {
            switch (file_content[i]) {
                case '(':
                    printf("ALPHA: Token -> %s, Value -> (, Length -> 1\n", tok_type_as_string(TOK_LPAREN));
                    break;
                case ')':
                    printf("ALPHA: Token -> %s, Value -> ), Length -> 1\n", tok_type_as_string(TOK_RPAREN));
                    break;
                case '-':
                    if (file_content[i + 1] == '>') {
                        printf("ALPHA: Token -> %s, Value -> ->, Length -> 2\n", tok_type_as_string(TOK_ARROW));
                        i++;
                        break;
                    }

                    printf("ALPHA: Token -> %s, Value -> -, Length -> 1\n", tok_type_as_string(TOK_DASH));
                    break;
                case '>':
                    printf("ALPHA: Token -> %s, Value -> >, Length -> 1\n", tok_type_as_string(TOK_GT));
                    break;
                case '{':
                    printf("ALPHA: Token -> %s, Value -> {, Length -> 1\n", tok_type_as_string(TOK_LBRACE));
                    break;
                case '}':
                    printf("ALPHA: Token -> %s, Value -> }, Length -> 1\n", tok_type_as_string(TOK_RBRACE));
                    break;
                case ':':
                    printf("ALPHA: Token -> %s, Value -> :, Length -> 1\n", tok_type_as_string(TOK_COLON));
                    break;
                case '=':
                    printf("ALPHA: Token -> %s, Value -> =, Length -> 1\n", tok_type_as_string(TOK_EQUAL));
                    break;
                case ';':
                    printf("ALPHA: Token -> %s, Value -> ;, Length -> 1\n", tok_type_as_string(TOK_SEMI));
                    break;
                default:
                    printf("Invalid delimiter...\n");
            }
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

    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        printf("ERROR: Could not open file -> %s\n", file_path); 
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    const size_t file_size = ftell(file);

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
