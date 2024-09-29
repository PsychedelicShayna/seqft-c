#ifndef _H_STACK
#define _H_STACK

#include <stdint.h>
#include <stdio.h>

#include "common.h"

typedef struct Stack Stack;

// Initialization.
// ----------------------------------------------------------------------------
extern Stack* Stack_new(size_t item_size);
extern Stack* Stack_withCapacity(size_t item_size, size_t count);
extern void   Stack_free(Stack* s);

// Resizing.
// ----------------------------------------------------------------------------
extern Stack* Stack_expandBy(Stack* s, size_t amount);
extern Stack* Stack_shrinkToFit(Stack* s);

// Mutating.
// ----------------------------------------------------------------------------
#define Stack_push(s, v) Stack_pushFrom(s, &(typeof(v)) {v});
extern Stack* Stack_pushFrom(Stack* s, void* item);
extern void*  Stack_pop(Stack* s);
extern Stack* Stack_clear(Stack* s);

// Retrieval.
// ----------------------------------------------------------------------------

// Memcpy's all the elements stored in the stack to a newlly allocated void*,
// and writes the void* to the dest void**, and returns the number of elements.
extern size_t Stack_cloneData(Stack* s, void** dest);

extern bool   Stack_empty(Stack* s);
extern void*  Stack_first(Stack* s);
extern void*  Stack_last(Stack* s);

// Getters.
// ----------------------------------------------------------------------------
extern void*  Stack_getBase(Stack* s);
extern void*  Stack_getHead(Stack* s);
extern size_t Stack_getCount(Stack* s);
extern size_t Stack_getItemSize(Stack* s);
extern size_t Stack_getCapacity(Stack* s);

// Debug.
// ----------------------------------------------------------------------------
extern Stack* Stack_print(Stack* s);

#endif // _H_STACK
