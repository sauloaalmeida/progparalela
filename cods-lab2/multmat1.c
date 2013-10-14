/*
Programa: multmat1.c
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: implementa a equacao C = C + A*B, usando a tecnica de blocking
*/

#include "matriz.h"
#define NITER 3

//funcao que multiplica duas matrizes: C[nXm] = C[nXm] + A[nXk] * B[kXm], usa a tecnica de blocking
int multMatriz1(double *A, double *B, double *C, int n, int m, int k, int NB) {
   int i, j, x;
   int i0, j0, x0;
   for(i=0; i<n; i+=NB)
      for(j=0; j<m; j+=NB)
         for(x=0; x<k; x+=NB)
           // mini-MMM loop nest (i0, j0, k0)
           for(i0=i; i0<(i + NB); i0++)
              for(j0=j; j0<(j + NB); j0++)
                 for(x0=x; x0<(x + NB); x0++)
                    C[(i0*m)+j0] += A[(i0*k)+x0] * B[(x0*m)+j0];
   return 1;
}

//funcao main
int main (int argc, char *argv[]) {
   struct timeval inicio, fim; //para tomada de tempo
   tsc_counter tsc1, tsc2; //para tomada de clocks
   long long unsigned int clock;
   double tempo;
   int j;
   int NB; //dimensao da mini-matriz quadrada usada para a tecnica de blocking
   int n, m, k, temp; //dimensoes das matrizes
   double *A, *B, *C; //matrizes de entrada e saida
   FILE *fA, *fB; //descritores de arquivo dos arquivos com as matrizes A e B

   //le e valida os argumentos de entrada
   //as matrizes A e B devem ser lidas de arquivos texto, onde a primeira linha contem o numero de linhas e colunas da matriz
   if(argc < 4) {
      printf("ERRO: lista de argumentos incorreta: \n<prog> <arq-matriz A> <arq-matriz B> <NB>\n");
      return -1;
   } 
   fA = fopen(argv[1], "r");
   if(fA==NULL) {printf("ERRO: leitura do arquivo com a matriz A\n"); return -1;}
   fB = fopen(argv[2], "r");
   if(fB==NULL) {printf("ERRO: leitura do arquivo com a matriz B\n"); return -1;}
   //le as dimensoes das matrizes
   if(fscanf(fA, "%d", &n)==0) return -1; //numero de linhas da matriz A
   if(fscanf(fA, "%d", &k)==0) return -1; //numero de colunas da matriz A
   if(fscanf(fB, "%d", &temp)==0) return -1; //numero de linhas da matriz B
   if(k!=temp)  {printf("ERRO: numero de colunas da matriz A deve ser igual ao numero de colunas da matriz B\n"); return -1;}
   if(fscanf(fB, "%d", &m)==0) return -1; //numero de colunas da matriz B

   NB =atoi(argv[3]); //tamanho dos blocos

   //aloca espaco de memoria para as matrizes
   A = malloc(sizeof(double) * (n*k));
   if(A==NULL) {printf("ERRO: alocação da matriz A\n"); return -1;}
   B = malloc(sizeof(double) * (k*m));
   if(B==NULL) {printf("ERRO: alocação da matriz B\n"); return -1;}
   C = malloc(sizeof(double) * (n*m));
   if(C==NULL) {printf("ERRO: alocação da matriz C\n"); return -1;}

   //carrega os valores iniciais das matrizes
   if(!carregaMatriz(A, fA, n, k)) return -1;
   //puts("Matriz A:");
   //imprimeMatriz(A, n, k);
   if(!carregaMatriz(B, fB, k, m)) return -1;
   //puts("Matriz B:");
   //imprimeMatriz(B, k, m);

   //fecha os arquivos de entrada
   fclose(fA);
   fclose(fB);

   //inicia a matriz C e faz uma multiplicacao inicial 
   inicMatriz(C, n, m);
   multMatriz1(A, B, C, n, m, k, NB);

   //mede o tempo de execução da funcao de multiplicacao (media de NITER repeticoes) 
   gettimeofday(&inicio, NULL); 
   RDTSC(tsc1);
   for (j=0; j<NITER; j++) {
      inicMatriz(C, n, m);
      multMatriz1(A, B, C, n, m, k, NB);
   }
   RDTSC(tsc2);
   gettimeofday(&fim, NULL);

   //puts("Matriz C:");
   //imprimeMatriz(C, n, m);
   
   //libera espaco de memoria das matrizes
   free(A);
   free(B);
   free(C);
   
   tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
   clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
   printf("Tempo: %.1lf(ms) Clocks: %.2e \n", tempo/NITER, (double)clock/NITER);
   printf("Clock/tempo: %.2e\n\n", clock/tempo);
   return 1;
}

