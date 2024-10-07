#include "visualizer.h"
#include "evaluator.h"
#include <unistd.h>

#define DEBUG

void Sft_draw(Sft* sft) {
#ifdef DEBUG
    system("clear");

    if(!sft || !sft->tarray)
        return;
    // Draw expression
    // -----------------------------------------------------------------------

    for(size_t i = 0; i < sft->tarray->count; ++i) {
        Token token = sft->tarray->tokens[i];

        char* as_string = Token_toString(&token);

        fprintf(stderr, "%s ", as_string);
    }

    fprintf(stderr, "\n");

    for(size_t i = 0; i < sft->tarray->count; ++i) {
        Token token = sft->tarray->tokens[i];

        if(i == sft->tarray_index) {
            fprintf(stderr, "^");
            break;
        } else {
            char* as_string = Token_toString(&token);

            size_t astr_len = strlen(as_string);

            for(size_t i = 0; i < astr_len; ++i) { fprintf(stderr, " "); }

            fprintf(stderr, " ");
        }
    }

    fprintf(stderr, "\n");

    // Matrix
    // =======================================================================

    // Initialize
    // -------------------------------
    char* matrix[12][2];
    char* empty = " ";

    for(int i = 0; i < 12; ++i) {
        matrix[i][0] = empty;
        matrix[i][1] = empty;
    }

    // Populate
    // -------------------------------
    Stack* ostack = sft->operator_stack;
    Stack* nstack = sft->number_stack;

    for(size_t i = 0; i < Stack_getCount(ostack); ++i) {
        if(i >= 12) {
            fprintf(stderr,
                    "Broke while populating matrix, operator stack index %zu "
                    "exceeded matrix size!\n",
                    i);
            sleep(2);
            break;
        }

        Token* token = Stack_itemAt(ostack, i);

        char* as_string = Token_toString(token);

        if(strlen(as_string) > 1) {
            as_string[1] = '\0';
        }

        matrix[i][0] = as_string;
    }

    for(size_t i = 0; i < Stack_getCount(nstack); ++i) {
        if(i >= 12) {
            fprintf(stderr,
                    "Broke while populating matrix, number stack index %zu "
                    "exceeded matrix size!\n",
                    i);
            sleep(2);
            break;
        }

        double* number = Stack_itemAt(nstack, i);

        char* as_string = (char*)malloc(256);
        memset(as_string, 0, 256);

        snprintf(as_string, 256, "%.2f", *number);

        matrix[i][1] = as_string;
    }

    // Draw
    // -------------------------------
    for(int i = 11; i >= 0; i--) {
        char* str_operator = matrix[i][0];
        char* str_number   = matrix[i][1];
        fprintf(stderr, "[%s] | [%s]\n", str_operator, str_number);

        if(str_operator != empty) {
            matrix[i][0] = empty;
        }

        if(str_number != empty) {
            matrix[i][1] = empty;
        }
    }

    fprintf(stderr, "\n");
#endif
}
