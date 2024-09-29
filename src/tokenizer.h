#ifndef _H_TOKENIZER_
#define _H_TOKENIZER_

#include <stdio.h>
enum TokenType {
    NUM = 0x00000001,

    ADD = 0x00000002, //: +
    SUB = 0x00000004, //: -
    DIV = 0x00000008, //: /
    MOD = 0x00000010, //: %
    MUL = 0x00000020, //: *
    POW = 0x00000040, //: ^

    OPERATORS = ADD | SUB | DIV | MOD | MUL | POW,

    OPAR = 0x00000080, //: (
    CPAR = 0x00000100, //: )
    PARS = OPAR | CPAR,

    INVALID = 0xFFFFFFFF
};

typedef struct Token {
    enum TokenType type;
    double         f64;
    char*          fn;
} Token;

typedef struct {
  char* message;
  size_t index;
} ParseError;

typedef struct {
  Token* tokens;
  size_t token_count;

  ParseError* errors;
  size_t error_count;
} TokenizeResult;

#endif // _H_TOKENIZER_
