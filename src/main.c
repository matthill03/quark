#include "lexer.h"
#include "parser.h"

void print_usage() {
    printf("USAGE: qkc <file_name>\n");
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
