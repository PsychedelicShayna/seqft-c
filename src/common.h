#ifndef _H_COMMON_
#define _H_COMMON_

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#define DEBUG
#include <stdio.h>

typedef int bool;
#define true (bool)1
#define false (bool)0

// Represents any non-recoverable error that can occur during iteration.
typedef struct IterErr {
    const char* message;
    size_t      index;
} IterErr;

extern char* filter_whitespace(const char* input, size_t len);

#endif // _H_COMMON_

