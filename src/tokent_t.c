#include "tokent_t.h"

#include <ctype.h>
#include <string.h>

tokent_t str_to_token_t(const char* str) {
  for(size_t i = 0; i < strlen(str); i++) {
    if(isdigit(str[i])) {
      return TT_NUMBER;
    }
  }

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
    {"|",  TT_OP_OR   },
    {"<<", TT_OP_LSH  },
    {">>", TT_OP_RSH  },
    {",",  TT_SY_COMA },
    {"(",  TT_SY_LPAR },
    {")",  TT_SY_RPAR },
  };

  for(size_t i = 0; i < sizeof(lookup_table) / sizeof(lookup_table[0]); ++i) {
    if(strcmp(str, lookup_table[i].name) == 0) {
      return lookup_table[i].type;
    }
  }

  return 0;
}
