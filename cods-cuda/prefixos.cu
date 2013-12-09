/* Disciplina: Ambientes de Programacao Paralela (MAB114) */
/* Prof.: Silvana Rossetto */
/* Codigo: Soma de prefixos em CUDA */

/* Para compilar: nvcc -o prefixo prefixo.cu */

#include <stdio.h>
#include <sys/time.h>

#define TAMANHO 65536
//#define PRINT

//funcao para execucao sequencial
void soma_prefixo_seq(int n, float *X) {
   int i;
   for(i=1; i<n; i++)
       X[i] = X[i-1] + X[i];
}

//Kernel para execucao paralela em CUDA (!!assume-se que o tamanho do vetor sera sempre potencia de 2!!)
__global__ void soma_prefixo_par(float *X) {
   int i = threadIdx.x;
   int n = blockDim.x;
   int offset;

   for(offset=1; offset<n; offset*=2) {
     int aux;
     if(i >= offset) { aux = X[i-offset]; }
     __syncthreads();
     if(i >= offset) { X[i] = aux + X[i]; }
     __syncthreads();
   }
}

//funcao para imprimir os elementos de um vetor de inteiros
void printVetor(float *vetor, int n) {
   for (int i=0; i<n; i++) 
      printf("%2.1f ", vetor[i]);
   printf("\n");
}

//funcao para computar um intervalo de tempo em milisegundos
double calculaTempo(struct timeval startTime, struct timeval endTime) {
   unsigned int totalMicroSecs;
   double t;
   totalMicroSecs = (unsigned long long) (endTime.tv_sec - startTime.tv_sec) * 1000000 +
            (unsigned long long) (endTime.tv_usec - startTime.tv_usec);
   t = (double) totalMicroSecs/1000;
   return t;
}

//funcao principal
int main(int argc, char** argv) {
   float *x, *cudaX;
   struct timeval inicio, fim;
   double tempo_seq, tempo_par;

   //aloca memoria na CPU
   x = (float*) malloc (sizeof(float) * TAMANHO);
   if(x==NULL) { printf("Erro malloc vetor X\n"); return(-1); }

   //inicialize o vetor de entrada
   for(int i=0; i<TAMANHO; i++){
      x[i] = (float) i;
   }

   //imprime o vetor de entrada
   #ifdef PRINT 
   printf("Vetor X\n"); 
   printVetor(x, TAMANHO);
   #endif

   //!!! ------------------------ executa sequencial ---------------------------------- !!!//
   gettimeofday(&inicio, NULL);
   soma_prefixo_seq(TAMANHO, x);
   gettimeofday(&fim, NULL);

   tempo_seq = calculaTempo(inicio, fim); // em milisegundos
   #ifdef PRINT 
   printf("Vetor X (calculo sequencial)\n");
   printVetor(x, TAMANHO);
   #endif

   //!!! ------------------------ executa em paralelo em CUDA -------------------------- !!!//
   //gettimeofday(&inicio, NULL);

   //aloca espaco para os vetores na GPU
   if (cudaMalloc (&cudaX, sizeof(float) * TAMANHO) != cudaSuccess) 
        { printf("Erro cudaMalloc\n"); return -1; }

   //copia os vetores de entrada da CPU para a GPU
   if (cudaMemcpy(cudaX, x, sizeof(float) * TAMANHO, cudaMemcpyHostToDevice) != cudaSuccess) 
        { printf("Erro cudaMemcpy\n"); return -1; }

   //dispara o kernel paralelo
   gettimeofday(&inicio, NULL);
   int nblocks = 1;  
   //!!!ATENCAO: pode causar erro se o tamanho do vetor extrapolar o numero maximo de threads por bloco!!!
   soma_prefixo_par <<<nblocks,TAMANHO>>> (cudaX);
   gettimeofday(&fim, NULL);

   //copia resultado da GPU para a CPU
   if (cudaMemcpy(x, cudaX, sizeof(float) * TAMANHO, cudaMemcpyDeviceToHost) != cudaSuccess) 
      { printf("Erro cudaMemcpy\n"); return -1; }
   
   //libera a memoria na GPU
   if (cudaFree(cudaX) != cudaSuccess) { printf("Erro cudaFree\n"); return -1; }

   //gettimeofday(&fim, NULL);
   tempo_par = calculaTempo(inicio, fim); // em milisegundos
   #ifdef PRINT 
   printf("Vetor X(calculo paralelo)\n");
   printVetor(x, TAMANHO);
   #endif
   
   //libera a memoria na CPU
   free(x);

   //------------------------------- imprime dos tempos de execucao ----------------------//
   printf("Tempo sequencial = %g mseg \n", tempo_seq);
   printf("Tempo paralelo = %g mseg \n", tempo_par);

   return 0;
}
