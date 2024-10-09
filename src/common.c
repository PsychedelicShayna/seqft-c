#include "common.h"
#include <alloca.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
    #define dprintf(s, ...) printf(s, ...)
#else
    #define dprintf(v, ...)
#endif

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

size_t filter_whitespace(const char* input, size_t len, char* dest) {
    char buffer[len + 2];
    memset(buffer, 0, len + 2);

    for(size_t i = 0, j = 0; i < len; ++i) {
        if(!isspace(input[i])) {
            buffer[j] = input[i];
            ++j;
        }
    }

    if(dest) {
        size_t buflen = strlen(buffer);
        memcpy(dest, buffer, buflen + 1);
        return buflen;
    } else {
        return 0;
    }
}

char* read_input(const char* prompt) {
    char* buffer = xmalloc(1);
    size_t size = 0;
    size_t len = 0;
    int c;

    if(prompt) {
        printf("%s", prompt);
    }

    while((c = fgetc(stdin)) != '\n') {
        if(len + 1 >= size) {
            size = size ? size * 2 : 256;
            buffer = realloc(buffer, size);
        }

        buffer[len++] = c;
    }

    buffer[len++] = '\0';
    return buffer;
}

void minmax(int64_t* n1, int64_t* n2, int64_t** min, int64_t** max) {
    if(*n1 < *n2) {
        *min = n1;
        *max = n2;
    } else {
        *min = n2;
        *max = n1;
    }
}

bool char_in(char character, const char* charset) {
    for(size_t i = 0; i < strlen(charset); ++i) {
        if(character == charset[i])
            return true;
    }

    return false;
}

char* hexdump(const void* in, size_t length, uint16_t group_by) {
    const size_t chunk = 1024 * 10;       // 10kb
    char* buffer = csrxmalloc(chunk); // uint16_t trick; 1 byte=2 in hex.
    size_t allocated = chunk;             // how much we malloc'd
    memset(buffer, 0, allocated);         // Zero initialize.

    size_t buffer_index = 0;
    uint64_t grouper = 1;

    for(size_t i = 0; i < length; ++i) {
        uint8_t* byte = (uint8_t*)in + i;
        uintptr_t offset = buffer_index * (sizeof(uint16_t)+1);
        char* destination = buffer + offset;

        if((uintptr_t)destination >= (uintptr_t)buffer + allocated) {
            allocated += chunk;
            buffer = realloc(buffer, allocated);
        }

        sprintf(destination, "%02x%c", *byte, (buffer_index+1) % group_by == 0 ? '\n' : ' ');
        buffer_index++;
        grouper++;

        if(grouper > group_by) {
            grouper = 1;
        }
    }

    return buffer;
}


