#ifndef TOKEN_H
#define TOKEN_H
#include <stdlib.h>

typedef struct TOKEN_STRUCT {
  enum {
    TOKEN_ID,
    TOKEN_EQUALS,
    TOKEN_STRING,
    TOKEN_SEMI,
    TOKEN_COLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COMMA,
    TOKEN_EOF,
  } type;

  char *value;

} token_T;

token_T *init_token(int type, char *value);

#endif // !TOKEN_H
