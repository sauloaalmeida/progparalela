#!/bin/bash

rm fft-versao8
gcc -O1 -lm -lpthread -o fft-versao8 fft-versao8.c

maxQtdThreads=4
maxElementos=25
for ((t=2; t<=$maxQtdThreads; t=t*2 ))
do
     for ((i=2; i<=$maxElementos; ++i ))
     do
	 for ((p=1; p<=$i-1; ++p ))
         do
              ./fft-versao8 $(( 2 ** i )) $t $p
         done
     done
done
