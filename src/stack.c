#include "stack.h"

Stack* Stack_New(size_t item_size) {
    Stack* s = (Stack*)malloc(sizeof(Stack));

    s->base       = 0;
    s->count      = 0;
    s->item_size  = item_size;
    s->capacity   = 0;
    s->_expand_by = 16;

    return s;
}

Stack* Stack_WithCapacity(size_t item_size, size_t amount) {
  Stack* s = Stack_New(item_size);
  size_t bytes_required = item_size * amount;
  s->base = malloc(bytes_required);
  s->capacity = bytes_required;
  return s;
}

void Stack_Free(Stack* s) {
    if(s && s->base) free(s->base);
    if(s) free(s);
}

void* Stack_Head(Stack* s) {
  void* head = s->base;

  if(s->count >= 1) {
    head += (s->count * s->item_size);
  }

  return head;
}

/// The amount is the amount of additional *items* the stack stack should have
/// enough space to store. The new base pointer of the stack is returned.
/// 0 will not result in a reallocation and return the existing base.
void* Stack_ExpandBy(Stack* s, size_t amount) {
    if(!amount)
        return s->base;

    size_t bytes_required = (s->count + amount) * s->item_size;

    void* new_base = malloc(bytes_required);
    memcpy(new_base, s->base, s->count * s->item_size);
    free(s->base);

    s->base     = new_base;
    s->capacity = bytes_required;

    return new_base;
}

void Stack_Push(Stack* s, void* item) {
    if((s->count + 1) * s->item_size > s->capacity) {
        Stack_ExpandBy(s, 1);
    }

    void* dest = Stack_Head(s) + s->item_size;
    memcpy(dest, item, s->item_size);

    s->count += 1;
}

// Reduces the item count by 1, and returns a copy of the element previously
// at the head of the stack.
void* Stack_Pop(Stack* s) {
    void* item_copy = malloc(s->item_size);
    void* head      = Stack_Head(s);

    memcpy(item_copy, head, s->item_size);

    s->count -= 1;
    return item_copy;
}

void Stack_Print(Stack* s) {
    printf(" | Item Count: %zu | Item Size: %zu | Capacity: %zu |  \n",
           s->count,
           s->item_size,
           s->capacity);

    printf("--------------------------------------------------------------\n");

    for(int i = 0; i < s->count; ++i) {
        void* item_location = s->base + (s->item_size * i);

        printf("%d: ", i);

        // Print item_size number of bytes in hex for each item.
        for(int b = 0; b < s->item_size; ++b) {
            char* byte = item_location + b;
            printf("%02x", *(unsigned*)byte);
        }

        printf("\n");
    }

    printf("--------------------------------------------------------------\n");
}
