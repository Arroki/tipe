#!/bin/sh

set -xe

# gcc -Wall -Wextra -fsanitize=address,undefined -o main main.c -Iinclude -lraylib -lm

# gcc -Wall -Wextra -fsanitize=address,undefined -o cell_lib.o cell_lib.c
#
#
#FLAGS="-Wall -Wextra -fsanitize=address -g -Iinclude"
FLAGS="-Wall -Wextra -Iinclude"
LIBS="-lraylib -lm"
OUT="build"

clang $FLAGS -c -o $OUT/dft.o dft.c
clang $FLAGS -c -o $OUT/epicycloid.o epicycloid.c
clang $FLAGS -o $OUT/out $OUT/dft.o $OUT/epicycloid.o main.c $LIBS
# gcc $FLAGS -o $OUT/chess $OUT/cell_lib.o $OUT/init.o main.c $LIBS
