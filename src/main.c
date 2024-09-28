#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"

// 0x00000001 0x00000002 0x00000004 0x00000008
// 0x00000010 0x00000020 0x00000040 0x00000080
// 0x00000100 0x00000200 0x00000400 0x00000800
// 0x00001000 0x00002000 0x00004000 0x00008000
// 0x00010000 0x00020000 0x00040000 0x00080000
// 0x00100000 0x00200000 0x00400000 0x00800000
// 0x01000000 0x02000000 0x04000000 0x08000000
// 0x10000000 0x20000000 0x40000000 0x80000000

#define true    1
#define false   0
#define nullptr 0
#define bool    int

enum TokenType {
    FLOAT = 0x00000001,

    ADD = 0x00000002, /*    +    */
    SUB = 0x00000004, /*    -    */
    DIV = 0x00000008, /*    /    */
    MOD = 0x00000010, /*    %    */
    MUL = 0x00000020, /*    *    */
    POW = 0x00000040, /*    ^    */

    OPERATORS = ADD | SUB | DIV | MOD | MUL | POW,

    LPAR = 0x00000080,
    RPAR = 0x00000100,
    PARS = LPAR | RPAR,

    INVALID = 0xFFFFFFFF
};

typedef struct Token {
    enum TokenType type;
    double         f64;
    char*          fn;
} Token;

void inc_or_panic(const char* ident1,
                  size_t*     idx,
                  const char* ident2,
                  size_t      max) {
    if(++(*idx) >= max || idx == 0) {
        fprintf(stderr,
                "%s := (%zu) out of bounds of %s := (%zu)",
                ident1,
                *idx,
                ident2,
                max);
        exit(1);
    }
}

typedef struct Vec {
    void*  bytes;
    size_t count;
    size_t t_size;
    size_t allocated;
} Vec;

void v_push(Vec* v, void* src) {
    if(v->count + 1 >= v->allocated) {
        size_t new_size = v->allocated != 0 ? v->allocated * 2 : 2;
        v->bytes        = realloc(v->bytes, new_size);
    }

    void* dst = (v->bytes + (v->t_size * v->count));
    memcpy(dst, src, v->t_size);
}

Vec* v_new(size_t t_size, size_t capacity) {
    Vec* vec       = (Vec*)malloc(sizeof(Vec));
    vec->bytes     = (void*)malloc(t_size * capacity);
    vec->count     = 0;
    vec->t_size    = t_size;
    vec->allocated = capacity;
    return vec;
}

void v_clear(Vec* v) {
    if(v->bytes)
        free(v->bytes);
    v->allocated = 0;
    v->count     = 0;
}

Token* tokenize(const char* expr, size_t len) {
    Vec* v_tokens = v_new(sizeof(Token), len);

    Vec *v_digits = v_new(sizeof(char), len),
        *v_ident  = v_new(sizeof(char), len);

    size_t counted_dot = false;

    for(int i = 0; i < len; ++i) {
        char c = expr[i];

        // tokens could start with letters if ending with ( ^ fn could contain
        // nums check digits first, and check if func name being built.

        if(isdigit(c) && !v_ident->count) {
            v_push(v_digits, &c);
            continue;
        }

        if((!counted_dot || !isdigit(c) && v_digits->count)) {
            double digit = atof(v_digits->bytes);
            Token  token = {.type = FLOAT, .f64 = digit, .fn = nullptr};
            v_push(v_tokens, &token);
            v_clear(v_digits);
            continue;
        }

        // Function name.
        if(isalnum(c)) {
            v_push(v_ident, &c);
            continue;
        }

        // Operator.
        else {
            switch(c) {}
        }
    }
}

int main() {
    Stack* test = Stack_WithCapacity(sizeof(uint32_t), 20);

    // Stack_Print(test);

    uint32_t x = 10;

    Stack_Push(test, &x);
    printf("Pushed: %d\n", *(uint32_t*)Stack_Head(test));

    // Stack_Print(test);

    x = 21;
    Stack_Push(test, &x);
    printf("Pushed: %d\n", *(uint32_t*)Stack_Head(test));

    x = 24;
    Stack_Push(test, &x);
    printf("Pushed: %d\n", *(uint32_t*)Stack_Head(test));

    // Stack_Print(test);

    uint32_t* y = (uint32_t*)Stack_Pop(test);
    printf("Popped: %d\n", *y);

    // Stack_Print(test);
    y = (uint32_t*)Stack_Pop(test);
    printf("Popped: %d\n", *y);

    // Stack_Print(test);
    y = (uint32_t*)Stack_Pop(test);
    printf("Popped: %d\n", *y);
    // Stack_Print(test);

}
