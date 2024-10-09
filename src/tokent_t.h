#ifndef _H_TOKEN_T_
#define _H_TOKEN_T_

#include <stddef.h>

typedef unsigned long tokent_t;

#define TT_NUMBER   0x00000001
#define TT_OP_ADD   0x00000002 //: +
#define TT_OP_SUB   0x00000004 //: -
#define TT_OP_DIV   0x00000008 //: /
#define TT_OP_MOD   0x00000010 //: %
#define TT_OP_MUL   0x00000020 //: *
#define TT_OP_POW   0x00000040 //: **
#define TT_OP_FLOOR 0x00000080 //: //
#define TT_OP_NEG   0x00000100 //: ~
#define TT_OP_AND   0x00000200 //: &
#define TT_OP_OR    0x00000400 //: |
#define TT_OP_XOR   0x00000800 //: ^
#define TT_OP_LSH   0x00001000 //: <<
#define TT_OP_RSH   0x00002000 //: >>
#define TT_OP_NOT   0x00004000 //: !
#define TT_SY_COMA  0x10000000 //: ,
#define TT_SY_LPAR  0x20000000 //: (
#define TT_SY_RPAR  0x40000800 //: )
                               
#define TT_OP_BITWISE \
  (TT_OP_AND | TT_OP_OR | TT_OP_XOR | TT_OP_RSH | TT_OP_LSH | TT_OP_NOT)

#define TT_UNARY_OPERATOR (TT_OP_NEG | TT_OP_NOT)

#define TT_BINARY_OPERATOR                                                           \
  (TT_OP_ADD | TT_OP_SUB | TT_OP_DIV | TT_OP_MOD | TT_OP_MUL | TT_OP_POW | TT_OP_AND \
   | TT_OP_OR | TT_OP_XOR | TT_OP_LSH | TT_OP_RSH | TT_OP_FLOOR)

#define TT_OPERATOR (TT_UNARY_OPERATOR | TT_BINARY_OPERATOR)
#define TT_SYMBOL   (TT_SY_LPAR | TT_SY_RPAR | TT_SY_COMA)

extern tokent_t str_to_token_t(const char* str);
extern size_t token_t_to_str(const tokent_t t, char* out, size_t size);
extern char* token_t_to_new_str(const tokent_t token_type);

typedef struct Token {
  tokent_t type;
  double f64;
  char* func;
  char str[256];
} Token;

extern char* Token_toString(Token* token);
extern void Token_print(Token* t);
extern void Token_freeMembers(Token* t);
extern void Token_free(Token* t);

typedef struct TokenArray {
  Token* tokens;
  size_t count;
} TokenArray;

extern TokenArray TokenArray_deepCopy(Token* tokens, size_t count);
extern void TokenArray_freeMembers(TokenArray* self);
extern void TokenArray_free(TokenArray* self);

#endif // _H_TOKEN_T_
