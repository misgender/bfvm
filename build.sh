#!/bin/sh
rm *.o main
gcc -I./include -c bfvm.c -o bfvm.o
gcc -I./include -c compile.c -o compile.o
gcc -I./include compile.o bfvm.o main.c -o main
