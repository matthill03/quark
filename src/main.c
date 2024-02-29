#include <stdio.h>
#include "include/lexer.h"

int main(int argc, char *arcv[]) {
  lexer_T* lexer = init_lexer(
    "set string::name = \"matt\";\n"
    "echo(name)"

  );

  token_T* token = (void*)0;

  while ((token = lexer_get_next_token(lexer)) != (void*)0) {
    printf("TOKEN(%d, %s)\n", token->type, token->value);
  }

  return 0;
}
