#ifndef _H_EVALUATOR_
#define _H_EVALUATOR_

#include "common.h"
#include "stack.h"
#include "tokenizer.h"

#include <math.h>

typedef struct {
    Stack* operator_stack;
    Stack* number_stack;
} Sft;

Sft* Sft_new() {
    Sft* sft = malloc(sizeof(Sft));

    sft->operator_stack = Stack_withCapacity(sizeof(Token*), 5);
    sft->number_stack   = Stack_withCapacity(sizeof(double), 5);

    Stack_setDeallocator(sft->operator_stack,
                         (void (*)(void*))Token_freeMembers);

    return sft;
}

void Sft_evalOps(Sft* s, Token* x) {
    Stack* opcellar  = s->operator_stack;
    Stack* numcellar = s->number_stack;

    while(!Stack_empty(opcellar)) {
        // Top of operator cellar.
        Token*    top   = Stack_last(opcellar);
        TokenType topty = top->type;

        if(x && x->type & TT_CPA && topty & TT_OPA) {
            Stack_pop(opcellar);
            break;
        }

        // if(x && x->type & TT_CPA && topty & TT_OPA)
        //     break;

        if(x && x->type & TT_OPS && topty < x->type)
            break;

        Token*    op = Stack_pop(opcellar);
        TokenType ot = op->type;

        double num3 = 0;

        if(ot & TT_UOP) {
            double* num = (double*)Stack_pop(numcellar);

            if(!num)
                break;

            if(ot & TT_NEG) {
              printf("Did negate\n");
                num3 = -(*num);
            }
        }

        else if(ot & TT_BOP) {

            double* num2 = (double*)Stack_pop(numcellar);
            double* num1 = (double*)Stack_pop(numcellar);

            if(!num1 || !num2) {
                break;
            }

            if(ot & TT_ADD)
                num3 = *num1 + *num2;

            else if(ot & TT_SUB)
                num3 = *num1 - *num2;

            else if(ot & TT_DIV)
                num3 = *num1 / *num2;

            else if(ot & TT_MUL)
                num3 = *num1 * *num2;

            else if(ot & TT_MOD)
                num3 = (uint64_t)*num1 % (uint64_t)*num2;

            else if(ot & TT_POW)
                num3 = pow(*num1, *num2);

        } else if(x && (x->type & TT_CPA)) {
            Token* head = Stack_getHead(opcellar);

            if(head && head->type & TT_OPA && head->func) {
              // Handle funcs
            }
        }

        Stack_pushFrom(numcellar, &num3);
    }
}

// Returns non-zero on error.
BOOL Sft_evalTokens(Sft* s, TokenArray* tokens, double* out_result) {
    // Stack_reClear(s->operator_stack);
    // Stack_reClear(s->number_stack);

    BOOL first_operator = TRUE;

    // Iterate from left to right.
    for(int i = 0; i < tokens->count; ++i) {
        Token* t = &tokens->tokens[i];

        // If X is a number, place X in the number cellar.
        if(t->type & TT_NUM) {
            Stack_pushFrom(s->number_stack, &t->f64);
        }

        // If token is an operator, evaluate operators until either
        // - Operator cellar is empty.
        //
        // - The top of the operator cellar is an open paren.
        //
        // - The precedence of the operator at the top of the operator
        //   cellar is LOWER than the precedence of t.
        else if(t->type & TT_OPS) {
            // The first time we encounter an operator, simply add it, no
            // evaluation.

            if(!first_operator) {
                Sft_evalOps(s, t);
            } else {
                first_operator = FALSE;
            }

            // Then place X in the cellar.
            Stack_pushFrom(s->operator_stack, t);
        }

        // If X is an open parenthesis, push X onto the operator cellar.
        else if(t->type & TT_OPA) {
            Stack_pushFrom(s->operator_stack, t);
        }

        // If X is a close parenthesis
        // - Evaluate operators until an open parenthesis is at the
        //   top of the operator cellar
        // - Remove the open parenthesis from the operator cellar.
        else if(t->type & TT_CPA) {
            Sft_evalOps(s, t);
        }
    }

    // If there are no more tokens to read, evaluate the remaining operators.
    if(!Stack_empty(s->operator_stack) && !Stack_empty(s->number_stack)) {
        Sft_evalOps(s, 0);
    }

    if(Stack_empty(s->number_stack)) {
        return 1;
    }

    double* result = Stack_pop(s->number_stack);

    if(result) {
        *out_result = *result;
        return 0;
    }

    return 1;
}

#endif // _H_EVALUATOR_
