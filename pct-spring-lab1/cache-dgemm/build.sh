#!/bin/sh

gcc -g -Wall -lm -o dgemm ./dgemm.c
# gcc -g -Wall -lm -o dgemm-unroll ./dgemm-unroll.c


gcc -Ofast -march=native -fprefetch-loop-arrays -funroll-loops -g -Wall -lm -o dgemm-unroll ./dgemm-unroll.c


