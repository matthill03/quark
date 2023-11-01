#include <stdio.h>
#include "include/lexer.h"
#include "include/parser.h"

int main(int argc, char* argv[])
{
    lexer_t* lexer = init_lexer(
        "set name = \"matt\";\n"
        "echo(name);\n"
    );

    parser_t* parser = init_parser(lexer);

    AST_t* root = parser_parse(parser);
   
    printf('hgoggoh\n');

    return 0;
}
