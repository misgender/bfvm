// bfvm.c: core virtual machine
// part of bfvm by Liz Cody <liz@cody.sh>

#include <stdio.h>
#include <stdlib.h>
#include "bfvm.h"

#define ADD 0x1000
#define INT 0x2000
#define BRZ 0x4000
#define BNZ 0x6000

#define MASK_ADD 0xfc00
#define MASK_ADD_DST 0x300
#define MASK_ADD_OP 0xff
#define MASK_JMP 0x8000
#define MASK_JMP_OP 0x7fff
#define MASK_INT 0xf000
#define MASK_INT_TARGET 0xf00
#define MASK_INT_PAYLOAD 0xff
#define MASK_BRANCH 0xe000
#define MASK_BRANCH_PAYLOAD 0x1fff

#define ADD_DST_IP 0
#define ADD_DST_DP 0x100
#define ADD_DST_DDP 0x300

#define INT_TARGET_HALT 0
#define INT_TARGET_PRINT 0x100
#define INT_TARGET_LOAD 0x200

Bfvm *bfvm_init() {
  Bfvm *vm = malloc(sizeof(*vm));

  if (vm == NULL) return NULL;

  vm->mem = calloc(65536, 1);

  if (vm->mem == NULL) {
    free(vm);
    return NULL;
  }

  vm->prog = NULL;
  vm->prog_size = 0;
  vm->ip = 0;
  vm->dp = 0;

  return vm;
}

BfvmStatus bfvm_step(Bfvm *vm) {
  if (vm->ip == vm->prog_size) return BS_HALT;
  if (vm->ip > vm->prog_size
      || vm->prog == NULL
      || vm->mem == NULL)
    return BS_FAULT;

  const BfvmOp op = vm->prog[vm->ip] & 0xffff;

  // nop:  0000 0000 0000 0000
  if (op == 0) goto nojmp;

  // add:  0001 00dd aaaa aaaa
  // rjmp: 0001 0000 aaaa aaaa
  if ((op & MASK_ADD) == ADD) {
    const int32_t operand = (op & MASK_ADD_OP) - 127;
    const BfvmOp dst = op & MASK_ADD_DST;

    // rjmp
    if (dst == ADD_DST_IP) {
      vm->ip += operand;
      return BS_OK;
    }

    // add
    if (dst == ADD_DST_DP) {
      vm->dp += operand;
      goto nojmp;
    }

    // add
    if (dst == ADD_DST_DDP) {
      vm->mem[vm->dp] += operand;
      goto nojmp;
    }

    // invalid dst
    return BS_FAULT;
  }

  // jmp:  1aaa aaaa aaaa aaaa
  if (op & MASK_JMP) {
    const uint16_t operand = op & MASK_JMP_OP;
    vm->ip = operand;
    return BS_OK;
  }

  // int:  0010 iiii aaaa aaaa
  if ((op & MASK_INT) == INT) {
    const uint32_t target = op & MASK_INT_TARGET;

    if (target == INT_TARGET_HALT) {
      return BS_HALT;
    }

    const uint32_t n = (op & MASK_INT_PAYLOAD) + 1;

    if (target == INT_TARGET_PRINT) {
      for (int i = 0; i < n; ++i) {
        putchar(vm->mem[vm->dp]);
      }
      goto nojmp;
    }

    if (target == INT_TARGET_LOAD) {
      for (int i = 0; i < n; ++i) {
        const int buf = getchar();
        if (buf == EOF) return BS_FAULT;
        vm->mem[vm->dp] = (uint8_t)buf;
      }
      goto nojmp;
    }
  }

  const BfvmOp brop = op & MASK_BRANCH;

  // brz:  010a aaaa aaaa aaaa
  if (brop == BRZ) {
    if (vm->mem[vm->dp] == 0) {
      const uint16_t payload = op & MASK_BRANCH_PAYLOAD;
      vm->ip = payload;
      return BS_OK;
    } else {
      goto nojmp;
    }
  }

  // bnz:  011a aaaa aaaa aaaa
  if (brop == BNZ) {
    if (vm->mem[vm->dp] != 0) {
      const uint16_t payload = op & MASK_BRANCH_PAYLOAD;
      vm->ip = payload;
      return BS_OK;
    } else {
      goto nojmp;
    }
  }

  // invalid op
  return BS_FAULT;

nojmp:
  ++vm->ip;
  return BS_OK;
}
