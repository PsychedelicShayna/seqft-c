#include "common.h"
#include <stdio.h>

// A wrapper to malloc that aborts the program immediately if malloc fails.
void* xmalloc(size_t size) {
    void* ptr = malloc(size);

    if(!ptr) {
        perror("Failed to malloc; out of memory.");
        abort();
    }
    return ptr;
}

// A wrapper to realloc that aborts the program immediately if realloc fails.
void* xrealloc(void* memory, size_t size) {
    void* ptr = realloc(memory, size);

    if(!ptr && size != 0) {
        perror("Failed to realloc; out of memory.");
        abort();
    }

    return ptr;
}

char* filter_whitespace(const char* input, size_t len) {
    char finput[len + 1];
    memset(finput, 0, len + 1);

    for(int i = 0, j = 0; i < len; ++i) {
        if(!isspace(finput[i])) {
            finput[j] = input[i];
            ++j;
        }
    }

    size_t flen = strlen(finput);

    char* result = (char*)csrxmalloc(flen + 1);
    memset(result, 0, flen + 1);
    memcpy(result, finput, flen);

    return result;
}

char* read_input(const char* prompt) {
    char*  buffer = xmalloc(1);
    size_t size   = 0;
    size_t len    = 0;
    int    c;

    if(prompt) {
        printf("%s", prompt);
    }

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


