#!/bin/bash

maxQtdThreads=4
maxElementos=25
for ((t=2; t<=$maxQtdThreads; t=t*2 ))
do
     for ((i=2; i<=$maxElementos; ++i ))
     do
	 for ((p=1; p<=$i-1; ++p ))
         do
              echo $(( 2 ** i )) $t $p
         done
     done
done
