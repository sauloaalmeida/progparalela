/* Disciplina: Ambientes de Programacao Paralela (MAB114) */
/* Prof.: Silvana Rossetto */
/* Codigo: Soma de vetores em CUDA */

/* Para compilar: nvcc -o somavetor somavetor.cu */

#include <stdio.h>
#include <sys/time.h>

#define TAMANHO 10000 
//#define PRINT_XY //imprime os vetores de entrada
#define PRINT //imprime o vetor de saida

//funcao para execucao sequencial
void calc_seq(int n, float alpha, float *X, float *Y) {
   int i;
   for(i=0; i<n; i++)
       Y[i] = alpha * X[i] + Y[i];
}

//Kernel para execucao paralela em CUDA
__global__ void calc_par(int n, float alpha, float *X, float *Y) {
   int i = blockIdx.x * blockDim.x + threadIdx.x;
   if(i < n) 
      Y[i] = alpha * X[i] + Y[i];
}

//funcao para imprimir os elementos de um vetor de inteiros
void printVetor(float *vetor, int n) {
   for (int i=0; i<n; i++) 
      printf("%.1f ", vetor[i]);
   printf("\n");
   printf("\n");
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
   float *y, *cudaY;
   float *x, *cudaX;
   float alpha;
   struct timeval inicio, fim;
   double tempo_seq, tempo_par;

   if(argc != 2) {
      puts("Erro: digite o nome do programa e o valor do coeficiente alpha");
      return -1;
   }
   //armazena parametro de entrada
   alpha = atof(argv[1]);

   //aloca memoria na CPU
   x = (float*) malloc (sizeof(float) * TAMANHO);
   if(x==NULL) { printf("Erro malloc vetor X\n"); return(-1); }
   y = (float*) malloc (sizeof(float) * TAMANHO);
   if(y==NULL) { printf("Erro malloc vetor Y\n"); return(-1); }

   //inicializa os dois vetores de entrada
   for(int i=0; i<TAMANHO; i++){
      x[i] = (float) i;
      y[i] = (float) i;
   }

   //imprime os vetores de entrada
   #ifdef PRINT_XY 
   printf("Vetor X\n"); 
   printVetor(x, TAMANHO);
   printf("Vetor Y\n");
   printVetor(y, TAMANHO);
   #endif

   //!!! ------------------------ executa sequencial ---------------------------------- !!!//
   gettimeofday(&inicio, NULL);
   calc_seq(TAMANHO, alpha, x, y);
   gettimeofday(&fim, NULL);

   tempo_seq = calculaTempo(inicio, fim); // em milisegundos
   #ifdef PRINT 
   printf("Vetor Y(calculo sequencial)\n");
   printVetor(y, TAMANHO);
   #endif


   //!!! ------------------------ executa em paralelo em CUDA -------------------------- !!!//
   //gettimeofday(&inicio, NULL);

   //inicializa os dois vetores de entrada
   for(int i=0; i<TAMANHO; i++){
      x[i] = (float) i;
      y[i] = (float) i;
   }
   //aloca espaco para os vetores na GPU
   if (cudaMalloc (&cudaX, sizeof(float) * TAMANHO) != cudaSuccess) 
        { printf("Erro cudaMalloc\n"); return -1; }
   if (cudaMalloc (&cudaY, sizeof(float) * TAMANHO) != cudaSuccess) 
        { printf("Erro cudaMalloc\n"); return -1; }

   //copia os vetores de entrada da CPU para a GPU
   if (cudaMemcpy(cudaX, x, sizeof(float) * TAMANHO, cudaMemcpyHostToDevice) != cudaSuccess) 
       { printf("Erro cudaMemcpy\n"); return -1; }
   if (cudaMemcpy(cudaY, y, sizeof(float) * TAMANHO, cudaMemcpyHostToDevice) != cudaSuccess) 
       { printf("Erro cudaMemcpy\n"); return -1; }

   //dispara o kernel paralelo
   gettimeofday(&inicio, NULL);
   int nblocks = (TAMANHO+255) / 256;
   calc_par <<<nblocks,256>>> (TAMANHO, alpha, cudaX, cudaY);
   gettimeofday(&fim, NULL);

   //copia resultado da GPU para a CPU
   if (cudaMemcpy(y, cudaY, sizeof(float) * TAMANHO, cudaMemcpyDeviceToHost) != cudaSuccess) 
       { printf("Erro cudaMemcpy\n"); return -1; }
   
   //libera a memoria na GPU
   if (cudaFree(cudaX) != cudaSuccess) { printf("Erro cudaFree\n"); return -1; }
   if (cudaFree(cudaY) != cudaSuccess) { printf("Erro cudaFree\n"); return -1; }

   //gettimeofday(&fim, NULL);
   tempo_par = calculaTempo(inicio, fim); // em milisegundos
   #ifdef PRINT 
   printf("Vetor Y(calculo paralelo)\n");
   printVetor(y, TAMANHO);
   #endif

   //libera a memoria na CPU
   free(x);
   free(y);

   //------------------------------- imprime dos tempos de execucao ----------------------//
   printf("Tempo sequencial = %g mseg \n", tempo_seq);
   printf("Tempo paralelo = %g mseg \n", tempo_par);

   return 0;
}
