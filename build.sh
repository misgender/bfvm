#!/bin/sh
rm *.o main
gcc -Wall -I./include -c vm.c -o vm.o
gcc -Wall -I./include -c comp.c -o comp.o
gcc -Wall -I./include comp.o vm.o main.c -o main
