#include "common.h"

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

    char* result = (char*)malloc(flen);
    memset(result, 0, flen+1);
    memcpy(result, finput, flen+1);

    return result;
}
