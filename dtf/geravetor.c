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

int potencia2(int p){
     int i,result;
     result = 1;
     for(i=0;i<p;i++)
           result = result*2;
          
      return result;     
}

//gera matriz quadrada de números double aleatórios
int geraVetorNumerosAleatorios (int n) {
   int i;
   
   srand(time(NULL)); /* inicializa gerador aleatorio */
   //imprime a primeira linha da matriz
   printf("%d\n", potencia2(n)/2); 
   //gera e imprime o conteúdo da matriz
   for(i=0; i<potencia2(n); i++) {
        /* rand() gera inteiro entre 0 e a constante RAND_MAX */
        printf("%.1lf ", (1.0*(rand()%MAX))/DIVISOR);
   } 
   return 0;
}

//gera matriz quadrada de números double não-aleatórios (recebe em n dimensão da matriz)
int geraVetorNumerosNaoAleatorios(int n) {
   int i;
   printf("%d\n", potencia2(n));
   //gera e imprime o conteúdo da matriz
   for(i=0; i<potencia2(n); i++) {
        printf("%.1lf ", 1.0*(i+1));
   } 
  return 1;
}

//função principal
int main(int argc, char *argv[]) {
   int n;
 
   if(argc < 2) {
      printf("ERRO: informe a potencia de base 2 do vetor que deverá ser gerada: <./prog> <dimensao>\n");
      return -1;
   }
   n = atoi(argv[1]); 

   //gera matriz quadrada de números nao-aleatorios
   //geraMatrizNumerosNaoAleatorios(n);
   
   //gera matriz quadrada de números aleatorios
   geraVetorNumerosAleatorios(n);
   
   return 1;
}
