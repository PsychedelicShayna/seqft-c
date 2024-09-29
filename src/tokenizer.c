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

TokenizeResult tokenize(const char* expr, size_t len) {
    Stack* token_stack = Stack_withCapacity(sizeof(Token), len);
    Stack* error_stack = Stack_withCapacity(sizeof(ParseError), 10);

    // Stores the characters that will eventually be turned into a token.
    Stack* char_stack = Stack_withCapacity(sizeof(char), len);

    bool found_point = false; // Has a decimal point been encountered?
    char base = 0; // 0 = decimal, x = hex, b = bin, o = oct, 0 = no base.

    // Initialize the operator to token type type map.
    enum TokenType type_map[256] = {0};
    memset(&type_map, 0, sizeof(enum TokenType) * 256);

    type_map['+'] = ADD;
    type_map['-'] = SUB;
    type_map['/'] = DIV;
    type_map['%'] = MOD;
    type_map['*'] = MUL;
    type_map['^'] = POW;
    type_map['('] = OPAR;
    type_map[')'] = CPAR;

    for(int i = 0; i < len; ++i) {
        char c = expr[i];


        // if(isalpha(c) && char-sta



    }

    Token* tokens = 0;
    size_t length = Stack_cloneData(token_stack, (void*)&tokens);

    Stack_free(token_stack);
    // return tokens;
}
