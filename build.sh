#!/bin/sh
rm *.o main
gcc -g -Wall -I./include -c vm.c -o vm.o
gcc -g -Wall -I./include -c comp.c -o comp.o
gcc -g -Wall -I./include comp.o vm.o main.c -o main
