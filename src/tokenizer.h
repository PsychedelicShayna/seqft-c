#ifndef _H_TOKENIZER_
#define _H_TOKENIZER_

#include <stdio.h>

#include "stack.h"
#include "tokent_t.h"

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

extern tokent_t TokenType_fromStr(const char* str);

typedef struct Token {
  tokent_t type;
  double   f64;
  char*    func;
  char     str[256];
} Token;

extern char* Token_toString(Token* token);
extern void  Token_print(Token* t);
extern void  Token_freeMembers(Token* t);
extern void  Token_free(Token* t);

typedef enum {
  ACC_NIL = 0x00000000, // Undetermined
  ACC_DEC = 0x00000001, // Decimal number
  ACC_BIN = 0x00000002, // Binary number
  ACC_FPN = 0x00000004, // Floating point number
  ACC_OCT = 0x00000008, // Octal number
  ACC_HEX = 0x00000010, // Hexadecimal number
  ACC_SPC = 0x00000020, // Special character, i.e. an operator or symbol.
  ACC_FUN = 0x00000080, // Function
  ACC_NUM = ACC_FPN | ACC_BIN | ACC_OCT | ACC_DEC | ACC_HEX,
  ACC_DTZ = 0x00001000, // Control bit set together with ACC_DEC that's set
                        // to indicate that the number has a trailing zero
                        // which needs to be resolved into something other
                        // than ACC_DEC by the next character to be valid.
} AccFlag;

extern AccFlag assign_accflag(char character, bool allow_hex);
extern AccFlag AccFlag_fromFormatChar(char character);

typedef struct TokenArray {
  Token* tokens;
  size_t count;
} TokenArray;

extern TokenArray TokenArray_deepCopy(Token* tokens, size_t count);
extern void       TokenArray_freeMembers(TokenArray* self);
extern void       TokenArray_free(TokenArray* self);

typedef struct Tokenizer {
  AccFlag  accflag;
  Stack*   char_stack;
  Stack*   token_stack;
  IterErr* error;
} Tokenizer;

// Returns a newly allocated string representing the token. Caller responsible
// for freeing char* returned from this function.
extern char* TokenType_toString(tokent_t t);

extern Tokenizer* Tokenizer_new(void);

extern bool Tokenizer_tokenize(Tokenizer* t, const char* expression, size_t expr_len,
                              TokenArray* out);

extern bool Tokenizer_parseStackAsNumber(Tokenizer* t, Token* out);

extern void Tokenizer_error(Tokenizer* t, const char* message, size_t expr_index);

extern void Tokenizer_clear(Tokenizer* t);
extern void Tokenizer_addToken(Tokenizer* t, Token* token);
extern void Tokenizer_free(Tokenizer* t);

#endif // _H_TOKENIZER_
