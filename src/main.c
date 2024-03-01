#include <stdio.h>
#include "include/ast.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/visitor.h"

int main(int argc, char *arcv[]) {
  lexer_T* lexer = init_lexer(
    "set string::name = \"matt\n\";\n"
    "set string::othername = \"rosie\n\";\n"
    "echo(name);"
    "echo(othername);"

  );

  parser_T* parser = init_parser(lexer);
  ast_T* root = parser_parse(parser);
  visitor_T* visitor = init_visitor();
  visitor_visit(visitor, root);

  return 0;
}
