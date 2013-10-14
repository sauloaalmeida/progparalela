/*
Programa: geramatrizes.c
Data: 7 de outubro de 2013
Descricao: gera matrizes quadradas para testes
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MAX 1000
#define DIVISOR 7

//gera matriz quadrada de números double aleatórios
int geraMatrizNumerosAleatorios (int n) {
   int i, j;
   
   srand(time(NULL)); /* inicializa gerador aleatorio */
   //imprime a primeira linha da matriz
   printf("%d %d\n", n, n); 
   //gera e imprime o conteúdo da matriz
   for(i=0; i<n; i++) {
     for(j=0; j<n; j++) 
        /* rand() gera inteiro entre 0 e a constante RAND_MAX */
        printf("%.1lf ", (1.0*(rand()%MAX))/DIVISOR);
     printf("\n");
   } 
   return 0;
}

//gera matriz quadrada de números double não-aleatórios (recebe em n dimensão da matriz)
int geraMatrizNumerosNaoAleatorios(int n) {
   int i,j;
   printf("%d %d\n", n, n);
   //gera e imprime o conteúdo da matriz
   for(i=0; i<n; i++) {
     for(j=0; j<n; j++) 
        printf("%.1lf ", 1.0*(i+1)*(j+1));
     printf("\n");
   } 
  return 1;
}

//função principal
int main(int argc, char *argv[]) {
   int n;
 
   if(argc < 2) {
      printf("ERRO: informe a dimensão da matriz quadrada que deverá ser gerada: <./prog> <dimensao>\n");
      return -1;
   }
   n = atoi(argv[1]); 

   //gera matriz quadrada de números nao-aleatorios
   //geraMatrizNumerosNaoAleatorios(n);
   
   //gera matriz quadrada de números aleatorios
   geraMatrizNumerosAleatorios(n);
   
   return 1;
}
