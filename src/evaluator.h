#ifndef _H_EVALUATOR_
#define _H_EVALUATOR_

#include "stack.h"
#include "tokenizer.h"

#include "visualizer.h"

typedef struct Function {
    char*  name;
    size_t args_req;
    double (*ptr)(double nums[], size_t len);
} Function;

extern double sft_round(double nums[], size_t len);

extern double sft_ceil(double nums[], size_t len);

typedef struct SftError {
    char message[256];
} SftError;

typedef struct Sft {
    Stack*     operator_stack;
    Stack*     number_stack;
    SftDrawer* drawer;

    TokenArray* tarray;
    size_t      tarray_index;

    SftError error;
} Sft;

extern Function FN_LOOKUP[2];

extern Sft* Sft_new(void);

extern double eval_binary_op(tokent_t operator_type, double num1, double num2);

extern double eval_unary_op(tokent_t operator_type, double num);

extern SftError* eval_x_is_operator(Sft* sft, Token token);

extern SftError* eval_x_is_close_paren(Sft* sft);

// Returns pointer to SftError stored internally in Sft instance on error.
// Does not allocate any new memory when returning an error. The Sft's
// SftError field is used as the "last error" buffer.
extern SftError* Sft_evalTokens(Sft* sft, TokenArray* tokens, double* out_result);

#endif // _H_EVALUATOR_
