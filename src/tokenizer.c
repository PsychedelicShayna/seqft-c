#include "tokenizer.h"
#include "common.h"
#include "stack.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 *       - Has the boolean boolean flag been tripped? If so, then is the letter
 *         permissible for the base?
 *         - If yes: Accumulate the letter.
 *         - If no:  Clear the chars stack, and add an error to the error stack.
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
 * base?
 *         - If yes: Accumulate the digit.
 *         - If no:  Clear the chars stack, and add an error to the error stack.
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
 *
 *
 *
 *
 * */

#ifdef DEEZNUTS
void tokenize(const char*     expr,
              size_t          len,
              TokenizeResult* out_result,
              ParseError*     out_error) {
    Stack* token_stack = Stack_withCapacity(sizeof(Token), len);

    // Stores the characters that will eventually be turned into a token.
    Stack* char_stack = Stack_withCapacity(sizeof(char), len);
    Stack* s          = char_stack; // Alias for char_stack.

    bool found_point = false; // Has a decimal point been encountered?
    bool func        = false; // Is the char stack a function name?
    char base        = 0;     // 0 = decimal, x = hex, b = bin, o = oct

    // Initialize the operator to token type type map.
    enum TokenType type_map[256] = {0};
    memset(&type_map, 0, sizeof(enum TokenType) * 256);

    type_map['+'] = TT_ADD;
    type_map['-'] = TT_SUB;
    type_map['/'] = TT_DIV;
    type_map['%'] = TT_MOD;
    type_map['*'] = TT_MUL;
    type_map['^'] = TT_POW;
    type_map['~'] = TT_NEG;
    type_map['('] = TT_OPA;
    type_map[')'] = TT_CPA;

    for(int i = 0; i < len; ++i) {
        char c = expr[i];

        bool   empty = Stack_empty(s);
        bool   alpha = isalpha(c);
        bool   digit = isdigit(c);
        size_t count = Stack_getCount(s);
        bool   op    = type_map[c];
        char*  first = Stack_first(s);

        // This should be impossible, but just in case.
        if(func && base) {
            ParseError error = {.message =
                                    "Something went very wrong. The func flag "
                                    "and number base flags are both tripped.",
                                .index = i};
            *out_error       = error;
            Stack_free(token_stack);
            Stack_free(char_stack);
            return;
        }

        // If the char stack is empty, and this isn't an operator, then either
        // way it's going to be added. Though the func flag is only tripped
        // if its a letter. This should only run once per token.
        if(empty && !op) {
            Stack_pushFrom(s, &c);
            func = alpha;
            continue;
        }

        // If the char stack has a count of 1, the func flag wasn't tripped,
        // the first character is a 0, and this is a letter, then it could be
        // the base of a number. If not, then it's an error.
        if(alpha && !func && !base && count == 1 && *first == '0') {
            if(c == 'x' || c == 'b' || c == 'o') {
                base = c;
                Stack_push(s, c);
                continue;
            } else {
                ParseError error = {.message = "Invalid base character.",
                                    .index   = i};

                *out_error = error;

                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }
        }

        // Continue accumulating function name.
        if(alpha && func && !base) {
            Stack_pushFrom(s, &c);
            continue;
        }

        // Continue accumulating number of a different base (hex only).
        if(alpha && !func && base == 'x') {
            if(c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F') {
                Stack_pushFrom(s, &c);
                continue;
            } else {
                ParseError error = {
                    .message = "Invalid character for hex base.", .index = i};
                *out_error = error;

                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }
        }

        // Accumulate binary numbers.
        if(digit && base == 'b') {
            if(c == '0' || c == '1') {
                Stack_pushFrom(s, &c);
                continue;
            } else {
                ParseError error = {
                    .message = "Invalid character for bin base.", .index = i};
                *out_error = error;

                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }
        }

        // Accumulate octal numbers.
        if(digit && base == 'o') {
            if(c >= '0' && c <= '7') {
                Stack_pushFrom(s, &c);
                continue;
            } else {
                ParseError error = {
                    .message = "Invalid character for oct base.", .index = i};
                *out_error = error;

                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }
        }

        // Since all the other cases have been handled, a digit can be safely
        // accumulated without checking for anything else. This may be part of
        // a hex number, or a decimal number, or a function name.
        if(digit) {
            Stack_pushFrom(s, &c);
            continue;
        }

        if(c == '.') {
            if(found_point) {
                ParseError error = {.message =
                                        "Multiple decimal points in number.",
                                    .index = i};
                *out_error       = error;

                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }

            if(func) {
                ParseError error = {
                    .message = "Decimal point in function name.", .index = i};
                *out_error = error;

                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }

            if(empty) {
                ParseError error = {.message = "Leading decimal point.",
                                    .index   = i};
                *out_error       = error;

                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }

            if(base) {
                ParseError error = {.message =
                                        "Decimal point in non-decimal number.",
                                    .index = i};
                *out_error       = error;

                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }

            found_point = true;
            Stack_pushFrom(s, &c);
            continue;
        }

        Token token = {.type = TT_NIL, .f64 = 0, .fn = 0};

        if(op) {
            token.type = type_map[c];

            if(token.type == TT_OPA && func) {
                token.fn = (char*)malloc(count + 1);
                memset(token.fn, 0, count + 1);
                memcpy(token.fn, Stack_getBase(s), count);
                Stack_clear(s);
            }

            Stack_push(token_stack, &token);
            Stack_clear(s);
            found_point = false;
            func        = false;
            base        = 0;
            continue;
        } else if(func && !isspace(c)) {
            ParseError error = {
                .message = "Function with no opening parenthesis.", .index = i};

            *out_error = error;

            Stack_free(token_stack);
            Stack_free(char_stack);
            return;
        }

        // It's not alpha, digit, or op, so it must be whitespace.
        // If it's not whitespace then it's an error.
        if(!isspace(c)) {
            ParseError error = {.message = "Invalid character.", .index = i};
            *out_error       = error;

            Stack_free(token_stack);
            Stack_free(char_stack);
            return;
        }

        // If it's not an operator, no function is being built, it's not
        // a decimal point, and the char stack is not empty, then unless
        // it's whitespace, it's an error. If it is whitespace, then parse
        // the number, and add it to the token stack.
        token.type = TT_NUM;

        if(found_point) {
            token.f64 = atof(Stack_getBase(s));
        } else if(base) {
            if(count < 3) {
                ParseError error = {.message =
                                        "Not enough digits to make a number.",
                                    .index = i};
                *out_error       = error;
                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }

            int base_int = base == 'x'   ? 16
                           : base == 'b' ? 2
                           : base == 'o' ? 8
                                         : 0;
            if(!base_int) {
                ParseError error = {.message = "Invalid base character.",
                                    .index   = i};

                *out_error = error;

                Stack_free(token_stack);
                Stack_free(char_stack);
                return;
            }

            Stack_push(s, '\0');

            // We want to skip +2 because strtoll can't understand all
            // prefixes for every base, only the 0x prefix for hex.
            token.f64 = strtoll(Stack_getHead(s) + 2, 0, base_int);
        }

        Stack_clear(s);
        Stack_pushFrom(token_stack, &token);
        func        = false;
        found_point = false;
        base        = 0;
        continue;
    }
}

#endif
void Token_print(Token* t) {
    char* b = TokenType_toString(t->type);

    printf("Token: {\n    type: %s,\n    f64: %f,\n    func: %s\n}\n",
           b,
           t->f64,
           t->func ? t->func : "null");

    free(b);
}

// Let's convert this mess into a struct with state.
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

bool valid_for_base(char c, AccFlag base) {
    switch(base) {
        case ACC_HEX:
            return isxdigit(c);
        case ACC_DEC:
            return isdigit(c);
        case ACC_OCT:
            return c >= '0' && c <= '7';
        case ACC_BIN:
            return c == '0' || c == '1';
        default:
            return isdigit(c);
    }
}

char* TokenType_toString(TokenType ttype) {
    char* buffer = malloc(100);
    memset(buffer, 0, 100);

    switch(ttype) {
        case TT_NUM:
            sprintf(buffer, "Number");
            break;
        case TT_ADD:
            sprintf(buffer, "Operator [ + ]");
            break;
        case TT_SUB:
            sprintf(buffer, "Operator [ - ]");
            break;
        case TT_DIV:
            sprintf(buffer, "Operator [ / ]");
            break;
        case TT_MOD:
            sprintf(buffer, "Operator [ %% ]");
            break;
        case TT_MUL:
            sprintf(buffer, "Operator [ * ]");
            break;
        case TT_POW:
            sprintf(buffer, "Operator [ ^ ]");
            break;
        case TT_NEG:
            sprintf(buffer, "Operator [ ~ ]");
            break;
        case TT_OPA:
            sprintf(buffer, "Operator [ ( ]");
            break;
        case TT_CPA:
            sprintf(buffer, "Operator [ ) ]");
            break;
        default:
            sprintf(buffer, "Unknown Token Type: %b", ttype);
            break;
    }

    return buffer;
}

Tokenizer* Tokenizer_new() {
    Tokenizer* t = malloc(sizeof(Tokenizer));
    memset(t, 0, sizeof(Tokenizer));

    t->tokens = Stack_withCapacity(sizeof(Token), 100);
    t->stacc  = Stack_withCapacity(sizeof(char), 100);
    t->accfl  = ACC_NIL;
    t->error  = 0;

    // Define char -> TokenType resolution.
    memset(t->tt_map, 0, sizeof(TokenType) * 256);
    t->tt_map['+'] = TT_ADD;
    t->tt_map['-'] = TT_SUB;
    t->tt_map['/'] = TT_DIV;
    t->tt_map['%'] = TT_MOD;
    t->tt_map['*'] = TT_MUL;
    t->tt_map['^'] = TT_POW;
    t->tt_map['~'] = TT_NEG;
    t->tt_map['('] = TT_OPA;
    t->tt_map[')'] = TT_CPA;

    return t;
}

void Tokenizer_free(Tokenizer* t) {
    if(t) {
        Stack_free(t->tokens);
        Stack_free(t->stacc);
        if(t->error) {
            free(t->error);
        }
        free(t);
    }
}

void Tokenizer_addToken(Tokenizer* t, Token* token) {
    Stack_pushFrom(t->tokens, token);
    Stack_clear(t->stacc);

#ifdef DEBUG
    printf("Added token to stack %s\n", TokenType_toString(token->type));
#endif

    t->accfl = ACC_NIL;
}

void Tokenizer_clear(Tokenizer* t) {
    Stack_clear(t->tokens);
    Stack_clear(t->stacc);
    t->accfl = ACC_NIL;
    if(t->error) {
        free(t->error);
        t->error = 0;
    }
}

void Tokenizer_error(Tokenizer* t, const char* message, size_t expr_index) {
    IterErr error = {.message = message, .index = expr_index};
    t->error      = malloc(sizeof(IterErr));
    *(t->error)   = error;
}

void Tokenizer_parseAccNum(Tokenizer* t) {
    Token token = {.type = TT_NUM, .f64 = 0, .func = 0};

    char buffer[Stack_getCount(t->stacc) + 1];
    memset(buffer, 0, Stack_getCount(t->stacc) + 1);
    memcpy(buffer, Stack_getBase(t->stacc), Stack_getCount(t->stacc));

    if(t->accfl & ACC_FPN) {
        token.f64 = atof(buffer);
    } else {
        int base = 10;

        if(t->accfl & (ACC_HEX | ACC_BIN | ACC_OCT)) {
            base = (int)t->accfl;
        }

        token.f64 = strtoll(buffer, 0, base);
    }

    Tokenizer_addToken(t, &token);
}

TokenizeResult* Tokenizer_parse(Tokenizer*  t,
                                const char* cexpr,
                                size_t      expr_len) {
    // Ensure the state of the tokenizer is clean.
    // ------------------------------------------------------------------------
    Tokenizer_clear(t);

    // First strip all whitespace from the expression.
    // ------------------------------------------------------------------------

    char expr[expr_len + 1];
    memset(expr, 0, expr_len + 1);

    for(int i = 0, j = 0; i < expr_len; ++i) {
        if(!isspace(cexpr[i])) {
            expr[j] = cexpr[i];
            ++j;
        }
    }

    expr_len = strlen(expr);

#ifdef DEBUG
    printf("Stripped Expression: '%s'(%zu). Originally '%s'(%zu)",
           expr,
           expr_len,
           cexpr,
           strlen(cexpr));
#endif

    // Now iterate through the expression.
    // ------------------------------------------------------------------------

    for(int i = 0; i < expr_len; ++i) {
        char c = expr[i];

        TokenType op = t->tt_map[c];

        // Finished accumulating a function.

        // It's an operator. Parse the accumulator, and add the operator token.
        // ------------------------------------------------------------------------
        if(op & TT_OPA && t->accfl & ACC_FUN) {
            Token token = {.type = TT_OPA, .f64 = 0, .func = 0};
            token.func  = (char*)malloc(Stack_getCount(t->stacc) + 1);
            memset(token.func, 0, Stack_getCount(t->stacc) + 1);
            memcpy(
                token.func, Stack_getBase(t->stacc), Stack_getCount(t->stacc));

            Tokenizer_addToken(t, &token);
        } else if(op & (TT_OPS | TT_PAS) && t->accfl & ACC_NUM) {
            Tokenizer_parseAccNum(t);
            Tokenizer_addToken(t, &(Token) {.type = op, .f64 = 0, .func = 0});
        } else if(op & (TT_OPS | TT_PAS)) {
            Token token = {.type = op, .f64 = 0, .func = 0};
            Tokenizer_addToken(t, &token);
        }

        // --------------------------------------------------------------------

        else if(isalpha(c) && t->accfl == ACC_NIL) {
            Stack_pushFrom(t->stacc, &c);
            t->accfl = ACC_FUN;
        }

        else if(isdigit(c) && t->accfl == ACC_NIL) {
            // If the first digit of an accumulation is 0, then there are some
            // rules that must be followed. A single zero is okay, but if a
            // next character exists and also a digit, then this 0 would be
            // a leading 0, which is not allowed in decimal numbers, but
            // if the next character is a valid base character, or a decimal
            // point, then the accfl is changed, and any future 0's are ok.
            printf("\nFirst hit at %d\n", i);
            t->accfl = ACC_DEC | (c == '0' ? ACC_DTZ : 0);
            Stack_pushFrom(t->stacc, &c);
        }

        else if(t->accfl & ACC_FUN) {
            Stack_pushFrom(t->stacc, &c);
        }

        else if(t->accfl & ACC_NUM) {
            if(Stack_getCount(t->stacc) == 1) {
                printf("\nSecond hit at %d\n", i);

                switch(c) {
                    case 'x':
                        t->accfl = ACC_HEX;
                        break;
                    case 'b':
                        t->accfl = ACC_BIN;
                        break;
                    case 'o':
                        t->accfl = ACC_OCT;
                        break;
                    case '.':
                        t->accfl = ACC_FPN;
                        break;
                    default:
                        printf("\nFinal hit: %d - %c\n", i, c);
                        break;
                }
            } else if((t->accfl & ACC_DEC) && c == '.') {
                t->accfl = ACC_FPN;
            } else if(!valid_for_base(c, t->accfl)) {
                Tokenizer_error(t, "Invalid character in number.", i);
                return 0;
            }

            if(t->accfl & ACC_DTZ) {
                Tokenizer_error(
                    t, "Leading zero in number is illegal in this context.", i);
                return 0;
            }

            Stack_pushFrom(t->stacc, &c);
        } else {
            Tokenizer_error(t, "How the hell did we get here?", i);
            return 0;
        }
    }

    // If there's any remaining elements in the accumulator, and
    // a function is being accumulated, then that's an error, since an open
    // paren is needed to complete it.
    //
    // A number is fine though.

    if(!Stack_empty(t->stacc)) {
        if(t->accfl & ACC_FUN) {
            Tokenizer_error(
                t, "Function with no opening parenthesis.", expr_len);
            return 0;
        } else if(t->accfl & ACC_NUM) {
            Tokenizer_parseAccNum(t);
        }
    }

    if(t->accfl & ACC_FUN) {
        Tokenizer_error(t, "Function with no opening parenthesis.", expr_len);
        return 0;
    }

    TokenizeResult* tkr = malloc(sizeof(TokenizeResult));
    tkr->tokens         = malloc(sizeof(Token) * Stack_getCount(t->tokens));

    memcpy(tkr->tokens,
           Stack_getBase(t->tokens),
           Stack_getCount(t->tokens) * sizeof(Token));

    tkr->token_count = Stack_getCount(t->tokens);

    // tokens->token_count    = Stack_cloneData(t->tokens,
    // (void*)&tokens->tokens);

    return tkr;
}
