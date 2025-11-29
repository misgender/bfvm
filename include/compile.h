// compile.h: types and functions for brainfuck compilation
// part of bfvm by Liz Cody <liz@cody.sh>

#pragma once

#include "bfvm.h"

ssize_t bfvm_compile(char*, size_t, BfvmOp**);
