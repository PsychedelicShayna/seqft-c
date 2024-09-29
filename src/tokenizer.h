#ifndef _H_TOKENIZER_
#define _H_TOKENIZER_

#include "stack.h"
#include <stdio.h>

typedef enum {
    TT_NUM = 0x00000001,
    TT_ADD = 0x00000002, //: +
    TT_SUB = 0x00000004, //: -
    TT_DIV = 0x00000008, //: /
    TT_MOD = 0x00000010, //: %
    TT_MUL = 0x00000020, //: *
    TT_POW = 0x00000040, //: ^
    TT_NEG = 0x00000080, //: ~
    TT_OPA = 0x00000100, //: (
    TT_CPA = 0x00000200, //: )
    TT_NIL = 0xFFFFFFFF,
    TT_OPS = TT_ADD | TT_SUB | TT_DIV | TT_MOD | TT_MUL | TT_POW | TT_NEG,
    TT_PAS = TT_OPA | TT_CPA,
} TokenType;

typedef struct Token {
    TokenType type;
    double    f64;
    char*     func;
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
    size_t token_count;
} TokenizeResult;

typedef struct Tokenizer {
    TokenType tt_map[256];
    AccFlag   accfl;
    Stack*    stacc; // haha, get it?... I'll see myself out.
    Stack*    tokens;
    IterErr*  error;
} Tokenizer;

extern void  Token_print(Token* t);
extern char* TokenType_toString(TokenType t);

extern Tokenizer*      Tokenizer_new();
extern TokenizeResult* Tokenizer_parse(Tokenizer*  t,
                                       const char* cexpr,
                                       size_t      expr_len);
extern void            Tokenizer_error(Tokenizer*  t,
                                       const char* message,
                                       size_t      expr_index);
extern void            Tokenizer_clear(Tokenizer* t);
extern void            Tokenizer_addToken(Tokenizer* t, Token* token);
extern void            Tokenizer_free(Tokenizer* t);

#endif // _H_TOKENIZER_
