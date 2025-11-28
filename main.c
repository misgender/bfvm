// main.c: cli frontend
// part of bfvm by Liz Cody <liz@cody.sh>

#include <stdio.h>
#include <stdlib.h>
#include "bfvm.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s <bf source file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  FILE *fd = fopen(argv[1], "rb");

  if (fd == NULL) {
    perror(argv[0]);
    return EXIT_FAILURE;
  }

  fclose(fd);

  int buf;

  while ((buf = fgetc(fd)) != EOF) {
  }
}
