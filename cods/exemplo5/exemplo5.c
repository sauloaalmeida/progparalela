/*
Programa: exemplo5.c
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: programa exemplo para avaliacao de desempenho em funcao do tamanho da entrada
*/

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

//Computa soma de prefixos de um vetor (versao 1)
void somap1 (float *a,  float *p, int n) {
   int i;
   p[0] = a[0]; 
   for (i=1; i<n; i++)
      p[i] = p[i-1] + a[i];      
}

//Computa soma de prefixos de um vetor (versao 2)
void somap2 (float *a,  float *p, int n) {
   int i;
   p[0] = a[0]; 
   for (i=1; i<n-1; i+=2) {
      float mid_val = p[i-1] + a[i];
      p[i] = mid_val;
      p[i+1] = mid_val + a[i+1];
   }
   //para o caso de n impar
   if(i<n)      
      p[i] = p[i-1] + a[i];
}
