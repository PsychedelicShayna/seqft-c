#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "stack.h"
#include "tokenizer.h"

void test_stack() {
    Stack* s = Stack_withCapacity(4, 100);
    Stack_print(s);

    Stack_shrinkToFit(s);

    Stack_push(s, 10);
    printf("Pushed: %d\n", *(uint32_t*)Stack_getHead(s));
    Stack_print(s);

    Stack_push(s, 21);
    printf("Pushed: %d\n", *(uint32_t*)Stack_getHead(s));
    Stack_print(s);

    Stack_push(s, 24);
    printf("Pushed: %d\n", *(uint32_t*)Stack_getHead(s));
    Stack_print(s);

    uint32_t* y = 0;

    y = Stack_pop(s);
    printf("Popped: %d\n", *y);
    Stack_print(s);

    y = Stack_pop(s);
    printf("Popped: %d\n", *y);
    Stack_print(s);

    y = Stack_pop(s);
    printf("Popped: %d\n", *y);
    Stack_print(s);
}

char* read_input() {
    char*  buffer = malloc(1);
    size_t size   = 0;
    size_t len    = 0;
    int    c;

    while((c = fgetc(stdin)) != '\n') {
        if(len + 1 >= size) {
            size   = size ? size * 2 : 256;
            buffer = realloc(buffer, size);
        }

        buffer[len++] = c;
    }

    buffer[len++] = '\0';
    return buffer;
}

void test_tokenizer(const char* expr) {
    Tokenizer* t = Tokenizer_new();

    size_t len = 0;

    len = strlen(expr);
    if(!len) {
        Tokenizer_free(t);
        return;
    }
    TokenizeResult* tr = Tokenizer_parse(t, expr, len);
    // Stack* ts = t->tokens;

    if(tr) {
        for(int i = 0; i < tr->token_count; ++i) {
            Token* t = &tr->tokens[i];
            Token_print(t);
        }
    } else {
        printf("Failed to tokenize.\n");



        if(t->error) {
            printf("error(i=%zu): %s\n---------------------------------\n",
                   t->error->index,
                   t->error->message);

            char* exprnw = filter_whitespace(expr, len);
            printf(">           %s\n", exprnw);
            free(exprnw);

            for(size_t i = 0; i < len+12; ++i) {
                if(i == t->error->index+12) {
                    printf("^");
                    break;
                } else {
                    printf(" ");
                }
            }
            printf("\n");

            for(size_t i = 0; i < len+12; ++i) {
                if(i == t->error->index+12) {
                    printf("|");
                    break;
                } else {
                    printf(".");
                }
            }

            printf("\n\n");
        }
    }

    Tokenizer_free(t);
}

int main() {

    while(true) {
        printf("Enter Expression: ");
        char* expr = read_input();

        test_tokenizer(expr);
    }
}
