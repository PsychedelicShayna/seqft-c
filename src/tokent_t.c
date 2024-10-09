#include "tokent_t.h"
#include "common.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const struct {
  const char* const name;
  tokent_t type;
} lookup_table[] = {
  {"+",  TT_OP_ADD  },
  {"-",  TT_OP_SUB  },
  {"/",  TT_OP_DIV  },
  {"%",  TT_OP_MOD  },
  {"*",  TT_OP_MUL  },
  {"^",  TT_OP_XOR  },
  {"**", TT_OP_POW  },
  {"~",  TT_OP_NEG  },
  {"//", TT_OP_FLOOR},
  {"&",  TT_OP_AND  },
  {"!",  TT_OP_NOT  },
  {"|",  TT_OP_OR   },
  {"<<", TT_OP_LSH  },
  {">>", TT_OP_RSH  },
  {",",  TT_SY_COMA },
  {"(",  TT_SY_LPAR },
  {")",  TT_SY_RPAR },
};

void Token_print(Token* self) {
  char* token_str = token_t_to_new_str(self->type);

  printf("Token: {\n    type: %s,\n    f64: %f,\n    func: %s\n}\n",
         token_str,
         self->f64,
         self->func ? self->func : "null");

  free(token_str);
}

char* Token_toString(Token* self) {
  memset(self->str, 0, sizeof(self->str));

  if(self->type == TT_SY_LPAR && self->func) {
    memcpy(self->str, self->func, strlen(self->func));
    self->str[strlen(self->func)] = '(';
  }

  else if(self->type & TT_NUMBER) {
    snprintf(self->str, sizeof(self->str), "%.2f", self->f64);
  }

  else if(self->type & (TT_OPERATOR|TT_SYMBOL)) {
    token_t_to_str(self->type, self->str, sizeof(self->str));
  }

  return self->str;
}

void Token_freeMembers(Token* t) {
  if(t && t->func) {
    free(t->func);
    t->func = 0;
  }
}

tokent_t str_to_token_t(const char* str) {
  for(size_t i = 0; i < strlen(str); i++) {
    if(isdigit(str[i])) {
      return TT_NUMBER;
    }
  }

  for(size_t i = 0; i < sizeof(lookup_table) / sizeof(lookup_table[0]); ++i) {
      printf("Checking '%s' against '%s'\n", str, lookup_table[i].name);
    if(strcmp(str, lookup_table[i].name) == 0) {
      return lookup_table[i].type;
    }
  }

  return 0;
}

size_t token_t_to_str(const tokent_t token_type, char* out, size_t size) {
  for(size_t i = 0; i < sizeof(lookup_table) / sizeof(lookup_table[0]); ++i) {
    if(token_type == lookup_table[i].type) {
      memset(out, 0, size);
      memcpy(out, lookup_table[i].name, size - 1);
      return strlen(out);
    }
  }

  return 0;
}

char* token_t_to_new_str(const tokent_t token_type) {
  char* buffer = csrxmalloc(1024);
  memset(buffer, 0, 1024);
  token_t_to_str(token_type, buffer, 1024);
  return realloc(buffer, strlen(buffer) + 1);
}

// Creates a new TokenArray from a deep copy of the provided `Token*` array of
// length `count`. TokenArray members (`Token*` tokens, and its members) is the
// callers responsibility to free via `TokenArray_freeMembers` / `TokenArray_free.`
TokenArray TokenArray_deepCopy(Token* tokens, size_t count) {
  TokenArray token_array;
  token_array.count = count;
  token_array.tokens = csrxmalloc(sizeof(Token) * count);
  memcpy(token_array.tokens, tokens, sizeof(Token) * count);

  for(size_t i = 0; i < token_array.count; ++i) {
    Token* token = &token_array.tokens[i];

    if(!token->func)
      continue;

    size_t func_size = strlen(token->func);
    char* func_copy = xmalloc(func_size + 1);
    strncpy(func_copy, token->func, func_size);
    func_copy[func_size] = '\0';
    token->func = func_copy;
  }

  return token_array;
}

void TokenArray_freeMembers(TokenArray* t) {
  if(t && t->tokens) {
    for(size_t i = 0; i < t->count; ++i) { Token_freeMembers(&t->tokens[i]); }

    free(t->tokens);
  }
}

void TokenArray_free(TokenArray* t) {
  if(t) {
    TokenArray_freeMembers(t);
    free(t);
  }
}
