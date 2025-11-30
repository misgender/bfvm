// main.c: cli frontend
// part of bfvm by Liz Cody <liz@cody.sh>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "bfvm.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <bf source file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int fd = open(argv[1], O_RDONLY);
  struct stat sb;
  fstat(fd, &sb);

  char *src = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  BfvmOp *ops;
  ssize_t ops_len = bfvm_compile(src, sb.st_size, &ops);
  munmap(src, sb.st_size);
  close(fd);

  Bfvm *bfvm = bfvm_init();
  bfvm_load(bfvm, ops, ops_len);
  BfvmStatus status;
  while ((status = bfvm_step(bfvm)) == BS_OK);
}
