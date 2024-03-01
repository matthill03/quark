#ifndef LEXER_H
#define LEXER_H
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "macros.h"
#include "token.h"

typedef struct LEXER_STRUCT {
  char c;
  unsigned int i;
  char *contents;
  int contents_size;

} lexer_T;

lexer_T *init_lexer(char *contents);

char lexer_peek(lexer_T* lexer, int offset);

void lexer_advance(lexer_T *lexer);

void lexer_skip_whitespace(lexer_T *lexer);

token_T *lexer_get_next_token(lexer_T *lexer);

token_T *lexer_collect_string(lexer_T *lexer);

token_T *lexer_collect_id(lexer_T *lexer);

token_T *lexer_advance_with_token(lexer_T *lexer, token_T *token);

char *lexer_get_current_char_as_string(lexer_T *lexer);

#endif // !LEXER_H
