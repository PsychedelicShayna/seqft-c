#ifndef _H_COMMON_
#define _H_COMMON_

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEBUG

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0 
#endif

// Represents any non-recoverable error that can occur during iteration.
typedef struct IterErr {
    const char* message;
    size_t      index;
} IterErr;

extern char* filter_whitespace(const char* input, size_t len);

// A wrapper to malloc that aborts the program immediately if malloc fails.
extern void* xmalloc(size_t size);

// A wrapper to realloc that aborts the program immediately if realloc fails.
extern void* xrealloc(void* memory, size_t size);

// An alias for xmalloc meaning "call site responsible" that explicitly states 
// that the caller of malloc is not responsible for freeing the memory, and
// that the corresponding free() should be found at the call site, or elsewhere. 
// Is functionally no different, only exists to make my life easier, and to
// help when searching for malloc/free pairs via ripgrep.
#define csrxmalloc xmalloc

#endif // _H_COMMON_

