#!/bin/bash

echo "------------------------------------------" >> /home/sandrade/log.txt
echo "Inicio multthread V1" >> /home/sandrade/log.txt
date >> /home/sandrade/log.txt

maxQtdThreads=4
maxElementos=26
for ((t=2; t<=$maxQtdThreads; t=t*2 ))
do
     for ((i=2; i<=$maxElementos; ++i ))
     do
	 for ((p=1; p<=$i-1; ++p ))
         do
              /home/sandrade/progparalela/fft/fft-paralela/apresentacao/fft-versao8 $(( 2 ** i )) $t $p >> /home/sandrade/log.txt
         done
     done
done

echo "Fim multthread V1" >> /home/sandrade/log.txt
date >> /home/sandrade/log.txt
echo "------------------------------------------" >> /home/sandrade/log.txt

echo " " >> /home/sandrade/log.txt
