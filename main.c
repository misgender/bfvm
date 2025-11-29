// main.c: cli frontend
// part of bfvm by Liz Cody <liz@cody.sh>

#include <stdio.h>
#include <stdlib.h>
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
  bfvm_compile(src, sb.st_size, NULL);
}
