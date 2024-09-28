#include <stdint.h>
#include <stdio.h>

typedef struct {
    void* base; // The most important pointer. The base of the stack.
                // This is where the memory allocated with malloc is.
                // All elements pushed to the stack can be found by
                // bottom + (item_size * item_index). The top of the
                // stack (very last element) can be calculated with
                // bottom + (item_size * item_count).

    size_t count; // How many items have been pushed onto the stack.

    size_t item_size; // How many bytes does each item occupy.
    size_t capacity;  // How many bytes are available past the base.

    size_t _expand_by; // When auto-expanding, how many new elements should be
                       // available? This number gets doubled per auto-expand.
} Stack;

extern Stack* Stack_New(size_t item_size);
extern Stack* Stack_WithCapacity(size_t item_size, size_t count);
extern void   Stack_Free(Stack* s);
extern void*  Stack_Head(Stack* s);
extern void*  Stack_ExpandBy(Stack* s, size_t amount);
extern void   Stack_Push(Stack* s, void* item);
extern void*  Stack_Pop(Stack* s);
extern void   Stack_Print(Stack* s);

