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
    //    printf("Pushed: %d\n", *(uint32_t*)Stack_getHead(s));
    Stack_print(s);

    Stack_push(s, 21);
    //    printf("Pushed: %d\n", *(uint32_t*)Stack_getHead(s));
    Stack_print(s);

    Stack_push(s, 24);
    //    printf("Pushed: %d\n", *(uint32_t*)Stack_getHead(s));
    Stack_print(s);

    uint32_t  _y = 0;
    uint32_t* y  = &_y;

    Stack_print(s);
    Stack_rePop(s, &_y);
    //    printf("Popped: %d\n", *y);

    Stack_print(s);
    Stack_rePop(s, &_y);
    //    printf("Popped: %d\n", *y);

    Stack_print(s);
    Stack_rePop(s, &_y);
    //    printf("Popped: %d\n", *y);
}

char* read_input() {
    char*  buffer = xmalloc(1);
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

    TokenArray* tr = Tokenizer_parse(t, expr, len);

    // Stack* ts = t->tokens;

    if(tr) {
        for(int i = 0; i < tr->count; ++i) {
            Token* t = &tr->tokens[i];
            Token_print(t);
        }

        TokenArray_freeMembers(tr);
        free(tr);
    }

    if(t->error) {
        //        printf("Failed to tokenize.\n");

        //        printf("error(i=%zu):
        //        %s\n---------------------------------\n",
        // t->error->index,
        // t->error->message);

        char* exprnw = filter_whitespace(expr, len);
        //        printf(">           %s\n", exprnw);
        free(exprnw);

        for(size_t i = 0; i < len + 12; ++i) {
            if(i == t->error->index + 12) {
                //                printf("^");
                break;
            } else {
                //                printf(" ");
            }
        }
        //        printf("\n");

        for(size_t i = 0; i < len + 12; ++i) {
            if(i == t->error->index + 12) {
                //                printf("|");
                break;
            } else {
                //                printf(".");
            }
        }

        //        printf("\n\n");
    }

    Tokenizer_free(t);
}

int main() {
    // test_stack();

    for(int i=0;i<100000000;++i) {
        test_tokenizer("1+1+1");
        test_tokenizer("1+1+1");
        test_tokenizer("0x10230");
        test_tokenizer("0x20f");
        test_tokenizer("0xz012");
        test_tokenizer("1*2+func(213)/0b123");
        test_tokenizer("10");
        test_tokenizer("10");
        test_tokenizer("10");
        test_tokenizer("213");
        test_tokenizer("1232^123");
        test_tokenizer("19");
        test_tokenizer("");
        test_tokenizer("");
        test_tokenizer("");
        test_tokenizer("         ");
        test_tokenizer("");
    }

    // while(TRUE) {
    //    //     printf("Enter Expression: ");
    //     char* expr = read_input();
    //
    //     test_tokenizer(expr);
    // }
}
