#include "tokenizer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "stack.h"
#include "tokent_t.h"

/*  Rules:
 *
 *  These operators are allowed and treated as single tokens: + - / % * ^
 *
 *  There can also exist:
 *    - Decimal Numbers:
 *      - No letter or leading zeros.
 *      - Can have a decimal point.
 *      - Can be prefixed with ~ in order to negate the number.
 *
 *    - Other Base Numbers:
 *      - The first character of the character stack must be 0.
 *      - The second character must be one of: x (hex), b (bin), o (oct).
 *      - The rest of the characters must be valid for the base
 *        - Hex: 0-9 a-f
 *        - Bin: 0-1
 *        - Oct: 0-7
 *      - The base character is copied into a char that is normally '\0',
 *         to indicate that the base has been found, so any letters after the
 *         base are treated as part of the number if allowed for that base.
 *         In this case, only hex numbers can have letters. Digits following
 *         the base must also be valid for the base.
 *
 *                 Logic
 *         -------------------------------------------------------------
 *
 *     - If letters are encountered, and the chars stack is empty, they are
 *       accumulated into the chars stack with no restrictions.
 *
 *     - If letters are encountered, and the chars stack is not empty, then
 *       the following checks are made:
 *
 *       - Is the length of the chars stack 1, and the first character a 0?
 *         - If yes: Is the second character a valid base character?
 *         - If yes: Accumulate the letter, and trip the boooolean flag.
 *
 *       - Has the BOOLean BOOLean flag been tripped? If so, then is the letter
 *         permissible for the base?
 *         - If yes: Accumulate the letter.
 *         - If no:  Clear the chars stack, and add an error to the error
 * stack.
 *
 *       - If not, then does the char stack lead with a digit? If so, then
 *       clear the chars stack, and add an error to the error stack.
 *
 *     - If digits are encountered, and the chars stack is empty, then they are
 *       accumulated into the chars stack.
 *
 *     - If digits are encountered, and the chars stack is not empty, then the
 *       following checks are made:
 *
 *       - Has a base been determined? If so, then is the digit valid for the
 *        base?
 *         - If yes: Accumulate the digit.
 *         - If no:  Clear the chars stack, and add an error to the error
 * stack.
 *
 *     - If non-digit, non-letter characters are encountered:
 *       - If it's an open parenthesis, and the char steck is not empty, and
 *         the base has not been determined, then add an open parenthesis token
 *         but treat the charstack as a function name, and copy it to the
 *         fn (function name) field.
 *
 *       - If it's anything else, then:
 *         - If the charstack is not empty, then parse the char stack as either
 *           a regular decimal number, or a number with a base if the base was
 *           determined. If it's not a number, then clear the char stack and
 *           add an error to the error stack.
 *
 *         - Always: add the token for the operator, if it's a valid operator.
 * */

// Note to self: implement a hash table.

AccFlag assign_accflag(char character, bool allow_hex) {
  if(isspace(character)) {
    return ACC_NIL;
  } else if(allow_hex ? isxdigit(character) : isdigit(character)) {
    return ACC_DEC | (character == '0' ? ACC_DTZ : 0);
  } else if(isalpha(character)) {
    return ACC_FUN;
  } else {
    return ACC_SPC;
  }
}

// Checks if the given digit is valid in the base set by AccFlag.
bool validate_digit(char digit, AccFlag base) {
  switch(base) {
    case ACC_HEX:
      return isxdigit(digit);
    case ACC_OCT:
      return digit >= '0' && digit <= '7';
    case ACC_BIN:
      return digit == '0' || digit == '1';
    default:
      return isdigit(digit);
  }
}

bool in_operator_charset(char c) {
  const char* charset = "<|'!>?/@#$%^&*_+=-\\:;,`~";
  return char_in(c, charset);
}

Tokenizer* Tokenizer_new() {
  Tokenizer* t = xmalloc(sizeof(Tokenizer));
  memset(t, 0, sizeof(Tokenizer));

  t->token_stack = Stack_withCapacity(sizeof(Token), 100);
  t->char_stack = Stack_withCapacity(sizeof(char), 100);

  Stack_setDeallocator(t->token_stack, (void (*)(void*))&Token_freeMembers);
  Stack_setDefaultAlloc(t->token_stack, 100);
  Stack_setDefaultAlloc(t->char_stack, 100);

  t->accflag = ACC_NIL;
  t->error = 0;

  return t;
}

void Tokenizer_free(Tokenizer* t) {
  if(t) {
    Stack_free(t->token_stack);
    Stack_free(t->char_stack);
    if(t->error) {
      free(t->error);
      t->error = 0;
    }
    free(t);
  }
}

void Tokenizer_addToken(Tokenizer* t, Token* token) {
  Stack_pushFrom(t->token_stack, token);
  Stack_clear(t->char_stack);

#ifdef DEBUG
  {
    char* token_str = token_t_to_new_str(token->type);

    if(token_str) {
      printf("Added token to stack %s\n", token_str);
      free(token_str);
    }
  }
#endif

  t->accflag = ACC_NIL;
}

void Tokenizer_clear(Tokenizer* t) {
  Stack_reClear(t->token_stack);
  Stack_reClear(t->char_stack);

  t->accflag = ACC_NIL;

  if(t->error) {
    free(t->error);
    t->error = 0;
  }
}

void Tokenizer_error(Tokenizer* t, const char* message, size_t expr_index) {
  IterErr error = {.message = message, .index = expr_index};

  if(t->error) {
    free(t->error);
    t->error = 0;
  }

  t->error = xmalloc(sizeof(IterErr));
  *(t->error) = error;
}

// Returns true on success.
bool Tokenizer_parseStackAsNumber(Tokenizer* self, Token* out) {
  if(!out) {
    Tokenizer_error(self, "Null Token* out parameter specified.", 0);
    return false;
  }

  Token token = {.type = TT_NUMBER, .f64 = 0, .func = 0};

  char* base_ptr = Stack_getBase(self->char_stack);
  size_t count = Stack_getCount(self->char_stack);

  if(!count) {
    Tokenizer_error(self, "Not enough digits to construct a number.", count);
    return false;
  }

  char buffer[count + 1]; // Nullbyte
  memset(buffer, 0, count + 1);

  char* begin = base_ptr;
  size_t len = count;

  // Futureproof against me adding any control bits in the future by
  // filtering out anything that isn't ACC_HEX, ACC_BIN, or ACC_OCT.
  int custom_base
    = (self->accflag & (~(self->accflag & ~(ACC_HEX | ACC_OCT | ACC_BIN))));

  bool is_float = self->accflag & ACC_FPN;

  int strtoll_base = custom_base ? custom_base : 10;

  if(custom_base || is_float) {
    if(count < 3) {
      Tokenizer_error(self, "Incomplete number.", count);

      return false;
    }

    // Exclude base specifier (0x, 0b, 0o, etc)
    if(custom_base) {
      begin += 2;
      len -= 2;
    }
  }

  memcpy(buffer, begin, len);

  if(is_float) {
    token.f64 = atof(buffer);
  } else {
    token.f64 = strtoll(buffer, 0, strtoll_base);
  }

  *out = token;
  return true;
}

AccFlag AccFlag_fromFormatChar(char character) {
  switch(character) {
    case 'x':
      return ACC_HEX;
    case 'b':
      return ACC_BIN;
    case 'o':
      return ACC_OCT;
    case '.':
      return ACC_FPN;
    default:
      return ACC_NIL;
  }
}

// Returns true on success, false on failure. TokenArray is written to out.
bool Tokenizer_tokenize(Tokenizer* self,
                        const char* expression,
                        size_t length,
                        TokenArray* out) {

  Tokenizer_clear(self);

  for(size_t i = 0; i < length; ++i) {
    char character = expression[i];

    if(isspace(character)) {
      continue;
    }

    AccFlag new_flag = assign_accflag(character, false);

    // ---------------------------------------------------------------
    // We start by determining what we're accumulating.
    // ---------------------------------------------------------------

    if(self->accflag == ACC_NIL) {
      AccFlag flag = assign_accflag(character, false);

      if(flag == ACC_NIL) {
        Tokenizer_error(self, "Could not classify character.", i);
        return false;
      }

      self->accflag = flag;
      Stack_pushFrom(self->char_stack, &character);
      continue;
    }

    // if(self->accfl == ACC_SPC && (isalpha(character) ||
    // isxdigit(character))) {
    //   Stack_pushFrom(self->char_stack, &(char) {'\0'});
    //   const char* characters = Stack_getBase(self->char_stack);
    //   tokent_t    token_type = str_to_token_t(characters);
    // }

    // Then we check if the character fits the rules of what's being
    // accumulated. If it doesn't, then it probably marks the start of the next
    // token, and so the character stack should be parsed into a Token of the
    // accumulation type.

    // It could be the next digit of a number.
    if(self->accflag & ACC_NUM) {
      bool valid_digit = validate_digit(character, self->accflag);

      if(valid_digit && self->accflag & ACC_DTZ) {
        Tokenizer_error(self, "Leading zero in number is illegal in this context.", 0);
        return false;
      }

      if(valid_digit) {
        Stack_pushFrom(self->char_stack, &character);
        continue;
      }

      // Perhaps it's a period '.' and the number hasn't been marked as a float yet?
      if(self->accflag == ACC_DEC && character == '.') {
        self->accflag = ACC_FPN;
        Stack_pushFrom(self->char_stack, &character);
        continue;
      }

      size_t stack_count = Stack_getCount(self->char_stack);

      // Perhaps it's the second character, it's a format specifier?
      if(self->accflag & ACC_DEC && stack_count == 1 && char_in(character, "xob.")) {
        AccFlag format_flag = AccFlag_fromFormatChar(character);

        if(format_flag == ACC_NIL) {
          Tokenizer_error(self, "Cannot determine the base format specifier.", i);
          return false;
        }

        self->accflag = format_flag;
        Stack_pushFrom(self->char_stack, &character);
        continue;
      }

      if(new_flag == ACC_NIL) {
        Tokenizer_error(self, "Cannot determine the type of token for character.", i);
        return false;
      }

      // Nope, it's just not valid. It's probably the start of the next token.
      // Try to parse the accumulated characters as a number and reset the accflag.
      Token number_possibly;
      Stack_push(self->char_stack, '\0'); // Ensure null termination.

      if(!Tokenizer_parseStackAsNumber(self, &number_possibly)) {
        Tokenizer_error(self, "Cannot parse number; invalid expression", i);
        return false;
      }

      // Ok, the number is valid.
      Stack_pushFrom(self->token_stack, &number_possibly);
      Stack_clear(self->char_stack);
      Stack_pushFrom(self->char_stack, &character);
      self->accflag = new_flag;
      continue;
    }

    // It could be the next letter in a function name.
    else if(self->accflag & ACC_FUN) {
      // Since the accflag has already been set to ACC_FUN, we know that the first
      // character must have been alphabetic, so subsequent characters are allowed
      // to be alphanumeric.

      bool valid_character = isalnum(character);

      if(valid_character) {
        Stack_pushFrom(self->char_stack, &character);
        continue;
      }

      // A function reaches its end when a open parenthesis is encountered, at
      // which point, an open parenthesis token with the function name copied
      // into it is pushed to the token stack.
      else if(character == '(') {
        Token token = {.type = TT_SY_LPAR, .f64 = 0, .func = 0};
        Stack_push(self->char_stack, '\0'); // Ensure null termination.

        const char* function_name = Stack_getBase(self->char_stack);
        size_t fn_length = strlen(function_name);

        token.func = csrxmalloc(fn_length + 1);
        strncpy(token.func, function_name, fn_length);
        token.func[fn_length] = '\0';

        Stack_clear(self->char_stack);

        // In this case we don't want to push the character onto the char stack,
        // since it's part of the token we've just pusheed to the token stack.
        // We also set accflag to NIL since the flag for this character does
        // not count as a new accumulation.
        Stack_pushFrom(self->token_stack, &token);
        self->accflag = ACC_NIL;

        continue;
      } else {
        Tokenizer_error(self, "Invalid character in function name.", i);
        return false;
      }
    }

    // It could be the next character of an operator or symbol.
    else if(self->accflag & ACC_SPC) {
      char* head = Stack_getHead(self->char_stack);

      bool valid_opchar = in_operator_charset(character);

      // We're accumulating a special character, and this character is
      // part of the valid character set, so simply push and continue;
      if(valid_opchar) {
        Token token = {.f64 = 0, .str = {0}, .func = 0, .type = 0};

        if((head && *head == '(') || *head == ')') {
          Stack_push(self->char_stack, '\0'); // Ensure null termination.
          const char* operator_str = Stack_getBase(self->char_stack);
          tokent_t operator_type = str_to_token_t(operator_str);
          if(!(operator_type & (TT_OPERATOR | TT_SYMBOL))) {
            Tokenizer_error(self, "3Cannot parse operator; invalid expression.", i);
            return false;
          }

          token.type = operator_type;
          Stack_pushFrom(self->token_stack, &token);
          Stack_clear(self->char_stack);
          Stack_pushFrom(self->char_stack, &character);
          continue;
        }
      }

      // Otherwise, we've hit the end of the operator or symbol, and need to
      // parse the char_stack as an operator, and push it to the token stack, if
      // valid.
      else {
        Token token = {.f64 = 0, .str = {0}, .func = 0, .type = 0};

        Stack_push(self->char_stack, '\0'); // Ensure null termination.
        const char* operator_str = Stack_getBase(self->char_stack);
        tokent_t operator_type = str_to_token_t(operator_str);

        printf("Section-----\n");
        printf("%16b\n", TT_OPERATOR);
        printf("%16b\n", TT_SYMBOL);
        printf("%16lb\n", operator_type);

        if(!(operator_type & (TT_OPERATOR | TT_SYMBOL))) {
          Tokenizer_error(self, "2Cannot parse operator; invalid expression.", i);
          return false;
        }

        token.type = operator_type;
        Stack_pushFrom(self->token_stack, &token);
        Stack_clear(self->char_stack);
        Stack_pushFrom(self->char_stack, &character);
        self->accflag = new_flag;
      }
    }

    // It's straight up invalid / not recognized..
    else {
      Tokenizer_error(self, "Invalid expression.", i);
      return false;
    }
  }

  // Handle remaining elements in the char stack.
  // ----------------------------------------------------------------------------------

  if(!Stack_empty(self->char_stack)) {
    Token token;

    if(self->accflag & ACC_NUM) {
      if(Tokenizer_parseStackAsNumber(self, &token)) {
        Stack_pushFrom(self->token_stack, &token);
        self->accflag = ACC_NIL;
      } else {
        Tokenizer_error(self, "Cannot parse number; invalid expression", length);
        return false;
      }
    } else if(self->accflag == ACC_FUN) {
      Tokenizer_error(self, "Function with no opening parenthesis.", length);
      return false;
    } else if(self->accflag == ACC_SPC) {
      Stack_push(self->char_stack, '\0'); // Ensure null termination.
      const char* operator_str = Stack_getBase(self->char_stack);
      tokent_t operator_type = str_to_token_t(operator_str);

      if(operator_type & (TT_OPERATOR | TT_SYMBOL)) {
        token.type = operator_type;
        Stack_pushFrom(self->token_stack, &token);
        Stack_clear(self->char_stack);
        self->accflag = ACC_NIL;
      } else {
        Tokenizer_error(self, "1Cannot parse operator; invalid expression.", length);
        return false;
      }
    }
  }

  TokenArray token_array = TokenArray_deepCopy(Stack_getBase(self->token_stack),
                                               Stack_getCount(self->token_stack));

  *out = token_array;

  Stack_reClear(self->token_stack);
  Stack_reClear(self->char_stack);

  return true;
}
