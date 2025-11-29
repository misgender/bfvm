// comp.c: compiler
// part of bfvm by Liz Cody <liz@cody.sh>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "bfvm.h"

enum tok_type {
  TOK_NIL,
  TOK_DP_INC,
  TOK_DP_DEC,
  TOK_DDP_INC,
  TOK_DDP_DEC,
  TOK_OUT,
  TOK_IN,
  TOK_LBRACE,
  TOK_RBRACE
};

struct tok {
  enum tok_type type;
  struct tok *counterpart;
  struct tok *prev_lbrace;
  struct tok *next;
};

static inline enum tok_type token_type(char ch) {
  switch (ch) {
    case '>': return TOK_DP_INC;
    case '<': return TOK_DP_DEC;
    case '+': return TOK_DDP_INC;
    case '-': return TOK_DDP_DEC;
    case '.': return TOK_OUT;
    case ',': return TOK_IN;
    case '[': return TOK_LBRACE;
    case ']': return TOK_RBRACE;
    default: return TOK_NIL;
  }
}

ssize_t bfvm_compile(char *src, size_t src_len,
                     BfvmOp **ops_out) {
  struct tok *head = NULL;
  struct tok *tail = NULL;
  struct tok *prev_lbrace = NULL;

  for (size_t i = 0; i < src_len; ++i) {
    char ch = src[i];
    enum tok_type tok_type = token_type(ch);
    if (tok_type == TOK_NIL) continue;
    if (head == NULL) {
      head = malloc(sizeof(*head));
      tail = head;
    } else {
      struct tok *new_tail = malloc(sizeof(*new_tail));
      tail->next = new_tail;
      tail = new_tail;
    }
    tail->type = tok_type;
    tail->prev_lbrace = prev_lbrace;
    tail->counterpart = NULL;
    tail->next = NULL;

    if (tok_type == TOK_RBRACE) {
      if (prev_lbrace == NULL) {
        fputs("dangling rbrace!", stderr);
        return -1;
      }
      tail->counterpart = prev_lbrace;
      prev_lbrace->counterpart = tail;
      prev_lbrace = prev_lbrace->prev_lbrace;
    }
    if (tok_type == TOK_LBRACE) {
      prev_lbrace = tail;
    }
  }

  for (struct tok *cur = head; cur != NULL; cur = cur->next) {
    printf("%d\n", cur->type);
  }

  return 0;

//  size_t ops_len = 0;
//  size_t ops_capacity = 512;
//  BfvmOp *ops = malloc(sizeof(*ops) * ops_capacity);
//
//  if (ops == NULL) return -1;
//
//  *ops_out = ops;
//  return ops_len;
}
