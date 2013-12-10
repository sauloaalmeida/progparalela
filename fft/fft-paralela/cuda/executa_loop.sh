#!/bin/bash


echo "------------------------------------------" >> /home/sandrade/log.txt
date >> /home/sandrade/log.txt
echo "Inicio Cuda" >> /home/sandrade/log.txt

maxQtdThreads=4096
maxElementos=26
for ((t=2; t<=$maxQtdThreads; t=t*2 ))
do
     for ((i=2; i<=$maxElementos; ++i ))
     do
         for ((tb=2;tb<=t;tb=tb*2))
         do
  	     for ((p=1; 2 ** $i > $t ** $p; ++p ))
             do
		  #echo "elementos:"$(( 2 ** i )) "threads:"$t "threadsPorBlocos:"$tb "peso:"$p
                  /home/sandrade/progparalela/fft/fft-paralela/cuda/fft-cuda $(( 2 ** i )) $t $tb $p >> /home/sandrade/log.txt
             done
         done
     done
done

echo "Fim Inicio Cuda" >> /home/sandrade/log.txt
date >> /home/sandrade/log.txt
echo "------------------------------------------" >> /home/sandrade/log.txt

echo " " >> /home/sandrade/log.txt
