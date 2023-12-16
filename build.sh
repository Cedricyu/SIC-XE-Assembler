#!/bin/bash

# Clean previous object files and executable
rm -f sym_node.o asm.o asm

# Compile sym_node.c
gcc -c sym_node.c -o sym_node.o

# Compile asm.c
gcc -c asm.c -o asm.o

# Link object files to create the executable
gcc asm.o sym_node.o -o asm
