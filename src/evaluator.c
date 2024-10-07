#include "evaluator.h"
#include "common.h"
#include "stack.h"
#include "tokenizer.h"
#include "tokent_t.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

double sft_round(double nums[], size_t len) {
    if(len < 1)
        return 0;

    double num = nums[0];
    return round(num);
}

double sft_ceil(double nums[], size_t len) {
    if(len < 1)
        return 0;

    double num = nums[0];
    return ceil(num);
}

#ifdef DEBUG
    #define debug_step(drawer, ...)   \
        Sft_draw(self);               \
        fprintf(stderr, __VA_ARGS__); \
        getchar();
#else
    #define debug_step(...)
#endif

Function FN_LOOKUP[2] = {
  (Function) {.ptr = sft_round, .name = "round", .args_req = 1},
   (Function) {.ptr = sft_ceil,  .name = "ceil",  .args_req = 1}
};

Sft* Sft_new(void) {
    Sft* self = malloc(sizeof(Sft));
    memset(self, 0, sizeof(Sft));

    self->operator_stack = Stack_withCapacity(sizeof(Token), 100);
    self->number_stack   = Stack_withCapacity(sizeof(double), 100);

    Stack_setDeallocator(self->operator_stack, (void (*)(void*))Token_freeMembers);

    return self;
}

double eval_binary_op(tokent_t operator_type, double num1, double num2) {
    if(operator_type == TT_OP_ADD)
        return num1 + num2;

    else if(operator_type == TT_OP_SUB)
        return num1 - num2;

    else if(operator_type == TT_OP_DIV)
        return num1 / num2;

    else if(operator_type == TT_OP_MUL)
        return num1 * num2;

    else if(operator_type == TT_OP_MOD)
        return (uint64_t)num1 % (uint64_t)num2;

    else if(operator_type == TT_OP_POW)
        return pow(num1, num2);
    else
        return 0xDEADC0DE;
}

double eval_unary_op(tokent_t operator_type, double num) {
    if(operator_type & TT_OP_NEG) {
        return -(num);
    }

    return 0xDEADC0DE;
}

SftError* eval_x_is_operator(Sft* self, Token token) {

    Stack* operator_cellar = self->operator_stack;
    Stack* number_cellar   = self->number_stack;

    while(1) {
        if(Stack_empty(operator_cellar))
            break;

        Token* top = Stack_getHead(operator_cellar);

        if(top->type & (TT_SY_LPAR | TT_SY_COMA))
            break;

        if(top->type < token.type)
            break;

        Token* operator_token = Stack_pop(operator_cellar);
        DEBUGBLOCK({ Sft_draw(self); });

        double result_to_push = 0;

        // Eval binary operators.
        if(operator_token->type & TT_BINARY_OPERATOR) {
            double* num2 = Stack_pop(number_cellar);
            DEBUGBLOCK({ Sft_draw(self); });

            double* num1 = Stack_pop(number_cellar);
            DEBUGBLOCK({ Sft_draw(self); });

            if(!num2 || !num1) {
                sprintf(self->error.message,
                        "Invalid expression, missing '%s' for binary operator"
                        "'%s'\n\n",
                        num1 ? "num1" : "num2", Token_toString(operator_token));

                return &self->error;
            }

            result_to_push = eval_binary_op(operator_token->type, *num1, *num2);
        }

        // Eval unary operators.
        else if(operator_token->type & TT_UNARY_OPERATOR) {
            double* num = Stack_pop(number_cellar);

            if(!num) {
                sprintf(self->error.message,
                        "Invalid expression, missing '%s' for unary operator "
                        "'%s'\n\n",
                        "num", Token_toString(operator_token));

                return &self->error;
            }

            DEBUGBLOCK({ Sft_draw(self); });

            result_to_push = eval_unary_op(operator_token->type, *num);
        }

        // Add calculation result to number cellar.
        Stack_pushFrom(number_cellar, &result_to_push);
        DEBUGBLOCK({ Sft_draw(self); });
    }

    return 0;
}

SftError* eval_x_is_close_paren(Sft* sft) {

    Stack* operator_cellar = sft->operator_stack;
    Stack* number_cellar   = sft->number_stack;

    size_t comas_encountered = 0;

    while(1) {
        double result_to_push = 0;

        if(Stack_empty(operator_cellar))
            break;

        Token* top = Stack_getHead(operator_cellar);

        if(top->type == TT_SY_LPAR) {
            if(top->func) {

                int found_func = 0;

                for(unsigned long i = 0; i < sizeof(FN_LOOKUP); ++i) {
                    Function* f = &FN_LOOKUP[i];

                    if(!strcmp(f->name, top->func)) {
                        found_func = 1;

                        if((comas_encountered + 1) != f->args_req) {
                            sprintf(sft->error.message,
                                    "Invalid number of arguments for function "
                                    "'%s', expected %zu, got %zu",
                                    f->name, f->args_req, comas_encountered + 1);

                            free(Stack_pop(operator_cellar));
                            return &sft->error;
                        }

                        if(Stack_getCount(number_cellar) < f->args_req) {
                            sprintf(sft->error.message,
                                    "Missing required number of arguments for "
                                    "function "
                                    "'%s', expected %zu, but only %zu values "
                                    "remain. ",
                                    f->name, f->args_req, comas_encountered + 1);

                            sprintf(sft->error.message,
                                    "Missing required number of arguments for "
                                    "function "
                                    "'%s', expected %zu, but only %zu values "
                                    "remain.",
                                    f->name, f->args_req, comas_encountered + 1);

                            free(Stack_pop(operator_cellar));
                            return &sft->error;
                        }

                        double fn_args[f->args_req];

                        for(size_t i = 0; i < f->args_req; ++i) {
                            double* num = Stack_pop(number_cellar);
                            if(!num) {
                                free(num);

                                sprintf(sft->error.message,
                                        "Failed to pop sufficient arguments "
                                        "from the number stack, '%s' expected "
                                        "%zu arguments, but only got %zu/%zu of "
                                        "the way through",
                                        f->name, f->args_req, i, f->args_req);

                                sprintf(sft->error.message,
                                        "Failed to pop sufficient arguments "
                                        "from the number stack"
                                        "'%s', expected %zu arguments, but "
                                        "only  got %zu/%zu of the way through.",
                                        f->name, f->args_req, i, f->args_req);

                                free(Stack_pop(operator_cellar));
                                return &sft->error;
                            }

                            fn_args[i] = *num;
                            free(num);
                        }

                        DEBUGBLOCK({ Sft_draw(self); });

                        double result = f->ptr(fn_args, f->args_req);
                        Stack_pushFrom(number_cellar, &result);
                        DEBUGBLOCK({ Sft_draw(self); });
                        break;
                    }

                    if(!found_func) {
                        sprintf(sft->error.message, "\nNo such function '%s'\n", top->func);
                        free(Stack_pop(operator_cellar));
                        return &sft->error;
                    }
                }

                free(Stack_pop(operator_cellar));
                DEBUGBLOCK({ Sft_draw(self); });
                break;
            }
        }
        Token* operator_token = Stack_pop(operator_cellar);
        DEBUGBLOCK({ Sft_draw(self); });

        if(operator_token->type == TT_SY_COMA) {
            comas_encountered++;
            continue;
        }

        // Eval binary operators.
        if(operator_token->type & TT_BINARY_OPERATOR) {
            double* num2 = Stack_pop(number_cellar);
            DEBUGBLOCK({ Sft_draw(self); });

            double* num1 = Stack_pop(number_cellar);

            DEBUGBLOCK({ Sft_draw(self); });

            if(!num2 || !num1) {
                sprintf(sft->error.message,
                        "Invalid expression, missing '%s' for binary operator "
                        "'%s'\n\n",
                        num1 ? "num1" : "num2", Token_toString(operator_token));

                return &sft->error;
            }

            result_to_push = eval_binary_op(operator_token->type, *num1, *num2);
        }

        // Eval unary operators.
        else if(operator_token->type & TT_UNARY_OPERATOR) {
            double* num = Stack_pop(number_cellar);

            if(!num) {
                sprintf(sft->error.message,
                        "Invalid expression, missing '%s' for unary operator "
                        "'%s'\n\n",
                        "num", Token_toString(operator_token));

                return &sft->error;
            }

            DEBUGBLOCK({ Sft_draw(self); });
            result_to_push = eval_unary_op(operator_token->type, *num);
        }

        // Add calculation result to number cellar.
        Stack_pushFrom(number_cellar, &result_to_push);
        DEBUGBLOCK({ Sft_draw(self); });
    }

    return 0;
}

// Returns pointer to SftError stored internally in Sft instance on error.
// Does not allocate any new memory when returning an error. The Sft's
// SftError field is used as the "last error" buffer.
SftError* Sft_evalTokens(Sft* self, TokenArray* tokens, double* out_result) {
    // SftDrawer* drawer = (SftDrawer*)malloc(sizeof(SftDrawer));
    // drawer->sft       = sft;
    // drawer->tarray    = tokens;
    // drawer->padding   = 0;

    self->tarray = tokens;

    // Iterate from left to right.
    for(size_t i = 0; i < tokens->count; ++i) {
        self->tarray_index = i;
        DEBUGBLOCK({ Sft_draw(self); });

        Token token = tokens->tokens[i];

        // If X is a number, place X in the number cellar.
        if(token.type & TT_NUMBER) {
            debug_step(drawer, "\n> Push Number\n");
            Stack_pushFrom(self->number_stack, &token.f64);
        }

        // If token is an operator, evaluate operators until either
        // - Operator cellar is empty.
        //
        // - The top of the operator cellar is an open paren.
        //
        // - The precedence of the operator at the top of the operator
        //   cellar is LOWER than the precedence of t.
        else if(token.type & (TT_OPERATOR | TT_SY_COMA)) {
            // The first time we encounter an operator, simply add it, no
            // evaluation.

            debug_step(drawer, "\n> Evaluate Stack\n");

            SftError* error = eval_x_is_operator(self, token);

            if(error) {
                return error;
            }

            // Then place X in the cellar.
            debug_step(drawer, "\n> Push Operator\n");
            Stack_pushFrom(self->operator_stack, &token);

        }

        // If X is an open parenthesis, push X onto the operator cellar.
        else if(token.type & TT_SY_LPAR) {
            debug_step(drawer, "\n> Push Operator\n");
            Stack_pushFrom(self->operator_stack, &token);
        }

        // If X is a close parenthesis
        // - Evaluate operators until an open parenthesis is at the
        //   top of the operator cellar
        // - Remove the open parenthesis from the operator cellar.
        else if(token.type & TT_SY_RPAR) {
            debug_step(drawer, "\n> Evaluate Stack\n");
            SftError* error = eval_x_is_close_paren(self);

            if(error) {
                return error;
            }
        }
    }

    debug_step(drawer, "\n> Evaluate Stack\n");

    // If there are no more tokens to read, evaluate the remaining
    // operators.
    while(1) {
        if(Stack_empty(self->operator_stack)) {
            break;
        }

        Token* operator_token = Stack_pop(self->operator_stack);

        double result_to_push = 0;

        // Eval binary operators.
        if(operator_token->type & TT_BINARY_OPERATOR) {
            double* num2 = Stack_pop(self->number_stack);
            double* num1 = Stack_pop(self->number_stack);

            if(!num2 || !num1) {
                sprintf(self->error.message,
                        "Invalid expression, missing '%s' for binary operator "
                        "'%s'\n\n",
                        num1 ? "num2" : "num1", Token_toString(operator_token));

                return &self->error;
            }

            result_to_push = eval_binary_op(operator_token->type, *num1, *num2);
        }

        // Eval unary operators.
        else if(operator_token->type & TT_UNARY_OPERATOR) {
            double* num = Stack_pop(self->number_stack);

            if(!num) {
                sprintf(self->error.message,
                        "Invalid expression, missing '%s' for unary operator "
                        "'%s'\n\n",
                        "num", Token_toString(operator_token));

                return &self->error;
            }

            result_to_push = eval_unary_op(operator_token->type, *num);
        }

        // Add calculation result to number cellar.
        Stack_pushFrom(self->number_stack, &result_to_push);
    }

    debug_step(drawer, "\n> Pop Result\n");
    double* result = Stack_pop(self->number_stack);
    DEBUGBLOCK({ Sft_draw(self); });

    if(result) {
        *out_result = *result;
        free(result);
    }

    return NULL;
}
