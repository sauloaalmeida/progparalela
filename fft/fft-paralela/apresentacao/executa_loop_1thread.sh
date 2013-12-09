#!/bin/bash

echo "------------------------------------------" > /home/sandrade/log.txt
echo "Inicio 1 thread" >> /home/sandrade/log.txt
date >> /home/sandrade/log.txt

maxElementos=26

for ((i=2; i<=$maxElementos; ++i ))
do
    /home/sandrade/progparalela/fft/fft-paralela/apresentacao/fft-versao8 $(( 2 ** i )) 1 1 >> /home/sandrade/log.txt
done

echo "Fim 1 thread" >> /home/sandrade/log.txt
date >> /home/sandrade/log.txt
echo "------------------------------------------" >> /home/sandrade/log.txt

echo " " >> /home/sandrade/log.txt
