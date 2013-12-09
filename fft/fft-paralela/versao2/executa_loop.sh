#!/bin/bash


echo "------------------------------------------" >> /home/sandrade/log.txt
date >> /home/sandrade/log.txt
echo "Inicio multthread V2" >> /home/sandrade/log.txt

maxQtdThreads=4
maxElementos=26
for ((t=2; t<=$maxQtdThreads; t=t*2 ))
do
     for ((i=2; i<=$maxElementos; ++i ))
     do
	 for ((p=1; p<=$i-1; ++p ))
         do
              /home/sandrade/progparalela/fft/fft-paralela/versao2/fft2 $(( 2 ** i )) $t $p >> /home/sandrade/log.txt
         done
     done
done

echo "Fim multthread V2" >> /home/sandrade/log.txt
date >> /home/sandrade/log.txt
echo "------------------------------------------" >> /home/sandrade/log.txt

echo " " >> /home/sandrade/log.txt
