#ifndef _H_TOKENIZER_
#define _H_TOKENIZER_

#include "stack.h"
#include <stdio.h>

// The three things preventing us from simply making a lookup table is
//
//  - Number accumulation.
//  - Number base determination.
//  - Function name accumulation & association with ( left parenthesis.
//    * In the future, functions with multiple arguments.
//
//  The state of the accumulator has to be checked for every charater in the
//  expression to ensure the character is compliant with the rules of what is
//  being accumulated, if something is being accumulated.
//
//  Something is accumulated if a number or a latter is detected. Operators
//  are single characters. We want to check operators first, since they will
//  always cause the accumulator to be cleared, and a token to be added to the
//  token stack, and in the case of left paren (, we can retrieve the function
//  name from the accumulator before we clear it.
//
//  After the operator check, we do our accumulator checks, and ensure that
//  the current character is compliant with the rules of the accumulation.
//
//  What is being accumulated is determined by AccFlag (accfl), and the rules
//  depend on the value of accfl. The initial state of accfl is NIL, but as
//  we iterate through the expression, its value will be determined, and we
//  must follow the rules.

typedef enum {
    TT_NUM      = 0x00000001,
    OPER_ADD    = 0x00000002, //: +
    OPER_SUB    = 0x00000004, //: -
    OPER_DIV    = 0x00000008, //: /
    OPER_MOD    = 0x00000010, //: %
    OPER_MUL    = 0x00000020, //: *
    OPER_POW    = 0x00000040, //: ^
    OPER_NEGATE = 0x00000080, //: ~  UNARY
    SYM_COMA    = 0x00010000, //: ,
    SYM_LPAR    = 0x00100000, //: (
    SYM_RPAR    = 0x00200000, //: )
    NULLTOKEN    = 0x02FFFFFF,
    OPERATORS_UNARY  = OPER_NEGATE,
    BINARY_OPERATOR_ =
        OPER_ADD | OPER_SUB | OPER_DIV | OPER_MOD | OPER_MUL | OPER_POW,
    OPERATOR_TOKENS = OPERATORS_UNARY | BINARY_OPERATOR_,
    SYMBOL_TOKENS   = SYM_LPAR | SYM_RPAR,
} TokenType;

typedef struct Token {
    TokenType type;
    double    f64;
    char*     func;
    char      str[256];
} Token;

typedef enum {
    ACC_NIL = 0x00000000, // Undetermined
    ACC_DEC = 0x00000001, // Decimal number
    ACC_BIN = 0x00000002, // Binary number
    ACC_FPN = 0x00000004, // Floating point number
    ACC_OCT = 0x00000008, // Octal number
    ACC_HEX = 0x00000010, // Hexadecimal number
    ACC_FUN = 0x00000080, // Function
    ACC_NUM = ACC_FPN | ACC_BIN | ACC_OCT | ACC_DEC | ACC_HEX,
    ACC_DTZ = 0x00001000, // Control bit set together with ACC_DEC that's set
                          // to indicate that the number has a trailing zero
                          // which needs to be resolved into something other
                          // than ACC_DEC by the next character to be valid.
} AccFlag;

typedef struct {
    Token* tokens;
    size_t count;
} TokenArray;

extern char* Token_toString(Token* token);

extern void Token_print(Token* t);

extern void Token_freeMembers(Token* t);
extern void Token_free(Token* t);

extern void TokenArray_freeMembers(TokenArray* t);
extern void TokenArray_free(TokenArray* t);

typedef struct Tokenizer {
    TokenType tt_map[256];
    AccFlag   accfl;
    Stack*    stacc; // haha, get it?... I'll see myself out.
    Stack*    tokens;
    IterErr*  error;
} Tokenizer;

// Returns a newly allocated string representing the token. Caller responsible
// for freeing char* returned from this function.
extern char* TokenType_toString(TokenType t);

extern Tokenizer*  Tokenizer_new();
extern TokenArray* Tokenizer_parse(Tokenizer*  t,
                                   const char* cexpr,
                                   size_t      expr_len);
extern BOOL        Tokenizer_parseAccNum(Tokenizer* t);
extern void        Tokenizer_error(Tokenizer*  t,
                                   const char* message,
                                   size_t      expr_index);
extern void        Tokenizer_clear(Tokenizer* t);
extern void        Tokenizer_addToken(Tokenizer* t, Token* token);
extern void        Tokenizer_free(Tokenizer* t);

#endif // _H_TOKENIZER_
