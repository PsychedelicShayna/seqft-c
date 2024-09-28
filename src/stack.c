#include <stdlib.h>
#include <string.h>

#include "stack.h"

typedef struct Stack {
    void*  base;
    void*  head;
    size_t count;
    size_t item_size;
    size_t capacity;
} Stack;

Stack* Stack_new(size_t item_size) {
    Stack* s = (Stack*)malloc(sizeof(Stack));

    s->base      = 0;
    s->head      = 0;
    s->count     = 0;
    s->item_size = item_size;
    s->capacity  = 0;
    return s;
}

Stack* Stack_withCapacity(size_t item_size, size_t amount) {
    Stack* s              = Stack_new(item_size);
    size_t bytes_required = item_size * amount;
    s->base               = malloc(bytes_required);
    s->head               = s->base;
    s->capacity           = bytes_required;
    return s;
}

void Stack_free(Stack* s) {
    if(s && s->base)
        free(s->base);
    if(s)
        free(s);
}

void* Stack_at(Stack* s, size_t index) {
    if(!s->count || index >= s->count)
        return 0;

    return s->base + (s->item_size * (index - 1));
}

// The amount is the amount of additional *items* the stack stack should have
// enough space to store. The new base pointer of the stack is returned.
// 0 will not result in a reallocation and return the existing base.
Stack* Stack_expandBy(Stack* s, size_t amount) {
    size_t bytes_required = (s->count + amount) * s->item_size;
    void*  new_base       = realloc(s->base, bytes_required);

    s->base     = new_base;
    s->capacity = bytes_required;
    s->head     = Stack_at(s, s->count - 1);

    return s;
}

Stack* Stack_shrinkToFit(Stack* s) {
  Stack_expandBy(s, 0);
  return s;
}

Stack* Stack_pushFrom(Stack* s, void* item) {
    if((s->count + 1) * s->item_size > s->capacity) {
        Stack_expandBy(s, 1);
    }

    void* dest = s->base + (s->item_size * s->count);
    memcpy(dest, item, s->item_size);

    s->head = dest;
    s->count += 1;
    return s;
}

// Reduces the item count by 1, and returns a copy of the element previously
// at the head of the stack. If the item count is 0, return 0.
void* Stack_pop(Stack* s) {
    if(s->count == 0)
        return 0;

    void* item_copy = malloc(s->item_size);
    memcpy(item_copy, s->head, s->item_size);

    s->head -= s->item_size;
    s->count -= 1;

    return item_copy;
}

Stack* Stack_print(Stack* s) {
    printf(" | Item Count: %zu | Item Size: %zu | Capacity: %zu |  \n",
           s->count,
           s->item_size,
           s->capacity);

    printf("--------------------------------------------------------------\n");

    for(int i = 0; i < s->count; ++i) {
        void* item_location = s->base + (s->item_size * i);

        printf("%d: |", i);

        // Print item_size number of bytes in hex for each item.
        for(int b = 0; b < s->item_size; ++b) {
            char* byte = item_location + b;
            printf("%02x|", *byte);
        }

        printf("\n");
    }

    printf("--------------------------------------------------------------\n");
    return s;
}

void* Stack_getBase(Stack* s) {
    return s->base;
}

void* Stack_getHead(Stack* s) {
    return s->head;
}

size_t Stack_getCount(Stack* s) {
    return s->count;
}

size_t Stack_getItemSize(Stack* s) {
    return s->item_size;
}

size_t Stack_getCapacity(Stack* s) {
    return s->capacity;
}
