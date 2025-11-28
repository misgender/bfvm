// bfvm.h: types and functions for bfvm
// part of bfvm by Liz Cody <liz@cody.sh>

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint16_t BfvmOp;

typedef struct bfvm_machine {
  BfvmOp *prog;
  size_t prog_size;
  uint16_t ip; // instruction pointer
  uint8_t *mem;
  uint16_t dp; // data pointer
} Bfvm;

typedef enum bfvm_status {
  BS_OK,
  BS_HALT,
  BS_FAULT,
} BfvmStatus;

Bfvm *bfvm_init();

static inline void
bfvm_load(Bfvm *vm, BfvmOp *prog,
          size_t prog_size) {
  vm->prog = prog;
  vm->prog_size = prog_size;
}

BfvmStatus bfvm_step(Bfvm*);
