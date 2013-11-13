#!/bin/bash

rm fft-thread
gcc -O1 -lm -lpthread -o fft-thread fft-thread.c
./fft-thread
