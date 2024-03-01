#include <stdio.h>
#include "include/ast.h"
#include "include/lexer.h"
#include "include/parser.h"

int main(int argc, char *arcv[]) {
  lexer_T* lexer = init_lexer(
    "set string::name = \"matt\";\n"
    "echo(name);"

  );

  parser_T* parser = init_parser(lexer);

  ast_T* root = parser_parse(parser);

  return 0;
}
