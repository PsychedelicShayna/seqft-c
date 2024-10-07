#ifndef _H_TOKEN_T_
#define _H_TOKEN_T_

#include <stddef.h>

typedef unsigned long tokent_t;

// Yes I obviously know what an enum is. This is deliberate. There's reasons.
#define TT_NUMBER          0x00000001
#define TT_OP_ADD          0x00000002 //: +
#define TT_OP_SUB          0x00000004 //: -
#define TT_OP_DIV          0x00000008 //: /
#define TT_OP_MOD          0x00000010 //: %
#define TT_OP_MUL          0x00000020 //: *
#define TT_OP_POW          0x00000040 //: ^
#define TT_OP_NEG          0x00000080 //: ~  UNARY
#define TT_SY_COMA         0x00010000 //: ,
#define TT_SY_LPAR         0x00100000 //: (
#define TT_SY_RPAR         0x00200000 //: )
#define TT_UNARY_OPERATOR  TT_OP_NEG
#define TT_BINARY_OPERATOR TT_OP_ADD | TT_OP_SUB | TT_OP_DIV | TT_OP_MOD | TT_OP_MUL | TT_OP_POW
#define TT_OPERATOR        TT_UNARY_OPERATOR | TT_BINARY_OPERATOR
#define TT_SYMBOL          TT_SY_LPAR | TT_SY_RPAR

tokent_t str_to_token_t(const char* str);

#endif // _H_TOKEN_T_
