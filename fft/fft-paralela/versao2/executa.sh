#!/bin/bash

rm fft2
gcc -O1 -lm -lpthread -o fft2 fft2.c
./fft2 8 2 2
