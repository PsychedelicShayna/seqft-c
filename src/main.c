#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "evaluator.h"
#include "stack.h"
#include "tokenizer.h"

char* read_stdin_pipe() {
    size_t size = 4096;
    char* buffer = malloc(size + 1);
    char* dest = buffer;

    size_t res = 0;
    while(true) {
        res = fread(dest, 1, 4096, stdin);
        if(!res)
            break;

        if(res == 4096) {
            size += 4096;
            buffer = realloc(buffer, size + 1);
            dest = buffer - 4096;
        }
    }

    buffer[size] = '\0';
    return buffer;
}
void test_stack() {
    Stack* s = Stack_withCapacity(4, 100);
    Stack_print(s);

    Stack_shrinkToFit(s);

    Stack_push(s, 10);
    printdbg("Pushed: %d\n", *((uint32_t*)Stack_getHead(s)));
    Stack_print(s);

    Stack_push(s, 21);
    printdbg("Pushed: %d\n", *(uint32_t*)Stack_getHead(s));
    Stack_print(s);

    Stack_push(s, 24);
    printdbg("Pushed: %d\n", *(uint32_t*)Stack_getHead(s));
    Stack_print(s);

    uint32_t _y = 0;
    uint32_t* y = &_y;

    Stack_print(s);
    Stack_rePop(s, &_y);
    printdbg("Popped: %d\n", *y);

    Stack_print(s);
    Stack_rePop(s, &_y);
    printdbg("Popped: %d\n", *y);

    Stack_print(s);
    Stack_rePop(s, &_y);
    printdbg("Popped: %d\n", *y);
}

void highlight_error(const char* expr, size_t expr_len, IterErr error, size_t indent) {

    printf("\n");

    char stripped[expr_len];
    filter_whitespace(expr, expr_len, stripped);

    const size_t bufsize = indent + expr_len + strlen(error.message) + 1;
    char padding[bufsize];
    memset(padding, 0, bufsize);

    memset(padding, ' ', indent);
    printf(">%s%s\n", padding, stripped);
    memset(padding, 0, bufsize);

    memset(padding, '~', indent + error.index);
    printf("%s ^\n%s\n", padding, error.message);
    memset(padding, 0, bufsize);

    memset(padding, '~', strlen(error.message));
    printf("%s\n", padding);
    memset(padding, 0, bufsize);

    printf("\n\n");
}

void test_sft(const char* expr) {
    Tokenizer* tokenizer = Tokenizer_new();
    Sft* sft = Sft_new();

    size_t expr_len = strlen(expr);

    if(!expr_len) {
        Tokenizer_free(tokenizer);
        return;
    }

    TokenArray token_array;

    if(!Tokenizer_tokenize(tokenizer, expr, expr_len, &token_array)) {
        highlight_error(expr, expr_len, *tokenizer->error, 2);
        Tokenizer_free(tokenizer);
    }

    if(tokenizer->error) {
        fprintf(stderr,
                "Error in tokenizer  %s, idx %zu",
                tokenizer->error->message,
                tokenizer->error->index);
        return;
    }

    // char buffer[256];
    //
    // for(int i = 0; i < token_array->count; ++i) {
    //     Token  t   = token_array->tokens[i];
    //     size_t len = Token_toString(&t, buffer);
    //     printf("Token '%s'\n", buffer);
    // }
    //
    // return;

#ifdef DEBUG
    for(size_t i = 0; i < token_array.count; ++i) {
        Token* t = &token_array.tokens[i];
        Token_print(t);
        if(t->func) {
            printf("Has func: %s\n", t->func);
        }
    }

    // TokenArray_freeMembers(token_array);
    // free(token_array);
#endif

    double result = 0;
    SftError* error = Sft_evalTokens(sft, &token_array, &result);

    if(error) {
        perror(error->message);
        abort();
    } else {
        printf("Result: %f\n", result);
    }

    Tokenizer_free(tokenizer);
}

void test_tokenizer(const char* expr) {
    Tokenizer* tokenizer = Tokenizer_new();

    size_t expr_len = strlen(expr);

    if(!expr_len) {
        Tokenizer_free(tokenizer);
        return;
    }
    TokenArray token_array;

#ifdef DEBUG

    for(size_t i = 0; i < token_array.count; ++i) {
        Token* t = &token_array.tokens[i];
        Token_print(t);
    }
#endif

    if(!Tokenizer_tokenize(tokenizer, expr, expr_len, &token_array)) {
        highlight_error(expr, expr_len, *tokenizer->error, 2);
        Tokenizer_free(tokenizer);
    }
}

int main() {
    // const char* expr = read_stdin_pipe();
    // test_sft(expr);
    // return 00;

    // test_stack();

    // for(int i=0;i<100000000;++i) {
    //     test_tokenizer("1+1+1");
    //     test_tokenizer("1+1+1");
    //     test_tokenizer("0x10230");
    //     test_tokenizer("0x20f");
    //     test_tokenizer("0xz012");
    //     test_tokenizer("1*2+func(213)/0b123");
    //     test_tokenizer("10");
    //     test_tokenizer("10");
    //     test_tokenizer("10");
    //     test_tokenizer("213");
    //     test_tokenizer("1232^123");
    //     test_tokenizer("19");
    //     test_tokenizer("");
    //     test_tokenizer("");
    //     test_tokenizer("");
    //     test_tokenizer("         ");
    //     test_tokenizer("");
    // }

    while(TRUE) {
        char* expr = read_input("(Debug):  ");
        test_sft(expr);
    }
}
