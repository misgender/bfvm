// comp.c: compiler
// part of bfvm by Liz Cody <liz@cody.sh>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <inttypes.h>
#include "bfvm.h"

#define ADD 0x1000
#define INT 0x2000
#define BRZ 0x4000
#define BNZ 0x6000

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
  int offset;
  int length;
  int index;
  char ch;
};

static inline int is_combining_token(char ch) {
  return ch == '+'
   || ch == '-'
   || ch == '<'
   || ch == '>'
   || ch == '.'
   || ch == ',';
}

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
  int nodeidx = 0;

  for (size_t i = 0; i < src_len; ++i) {
    char ch = src[i];
    enum tok_type tok_type = token_type(ch);
    if (tok_type == TOK_NIL) continue;
    if (tail != NULL
        && is_combining_token(ch)
        && tok_type == tail->type
        && tail->length < 127) {
      ++tail->length;
      continue;
    }
    if (head == NULL) {
      head = malloc(sizeof(*head));
      tail = head;
    } else {
      struct tok *new_tail = malloc(sizeof(*new_tail));
      tail->next = new_tail;
      tail = new_tail;
    }
    tail->index = nodeidx++;
    tail->offset = i;
    tail->type = tok_type;
    tail->prev_lbrace = prev_lbrace;
    tail->counterpart = NULL;
    tail->next = NULL;
    tail->length = 1;
    tail->ch = ch; // TODO: delet this?

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

  BfvmOp *ops = malloc(sizeof(*ops) * nodeidx);
  int opidx = 0;

  for (struct tok *tok = head; tok != NULL; tok = tok->next) {
#ifdef _BF_DEBUG
    printf("%d=%cx%d", tok->index, tok->ch, tok->length);
    if (tok->counterpart == NULL) {
      putchar('\n');
    } else {
      printf("(%d=%c)\n", tok->counterpart->index, tok->counterpart->ch);
    }
#endif
    if (tok->type == TOK_DP_INC
        || tok->type == TOK_DP_DEC
        || tok->type == TOK_DDP_INC
        || tok->type == TOK_DDP_DEC) {
      int reg = (tok->type == TOK_DP_INC
                 || tok->type == TOK_DP_DEC) ? 1 : 3;
      int sign = (tok->type == TOK_DP_DEC
                     || tok->type == TOK_DDP_DEC) ? -1 : 1;
      int payload = (sign * tok->length) + 127;
      ops[opidx++] = UINT16_C(ADD | (reg << 8) | (payload & 0xff));
    } else if (tok->type == TOK_OUT
              || tok->type == TOK_IN) {
      int irq = tok->type == TOK_OUT ? 1 : 2;
      ops[opidx++] = UINT16_C(INT | (irq << 8) | (tok->length & 0xff));
    } else if (tok->type == TOK_LBRACE) {
      ops[opidx++] = UINT16_C(BRZ | ((tok->counterpart->index + 1) & 0x7fff));
    } else if (tok->type == TOK_RBRACE) {
      ops[opidx++] = UINT16_C(BNZ | ((tok->counterpart->index + 1) & 0x7fff));
    } else {
      fputs("unknown token!!!\n", stderr);
      return -1;
    }
  }
  *ops_out = ops;
  return opidx;
}
