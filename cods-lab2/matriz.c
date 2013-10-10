/*
Biblioteca: matriz.c
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: implementa funcoes para manipulacao de matrizes
*/

#include "matriz.h"

//funcao que inicializa uma matriz com 0s
int inicMatriz(double *mat, int linhas, int colunas) {
   int i, j;
   for (i=0; i<linhas; i++)
      for (j=0; j<colunas; j++)
         mat[(i*colunas)+j] = 0;
   return 1;
}

//funcao que carrega uma matriz de um arquivo
int carregaMatriz(double *mat, FILE *fp, int linhas, int colunas) {
   int i, j;
   for (i=0; i<linhas; i++)
      for (j=0; j<colunas; j++)
         if(fscanf(fp, "%lf", &mat[(i*colunas)+j])==0) return 0;
   return 1;
}

//funcao que imprime uma matriz
int imprimeMatriz(double *mat, int linhas, int colunas) {
   int i, j;
   for (i=0; i<linhas; i++) {
      for (j=0; j<colunas; j++)
         printf("%.1lf ", mat[(i*colunas)+j]);
      printf("\n");
   }
   return 1;
}
