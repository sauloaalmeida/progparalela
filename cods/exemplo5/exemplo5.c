/*
Programa: exemplo5.c
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: programa exemplo para avaliacao de desempenho em funcao do tamanho da entrada
*/

#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#define NITER 100000000

//estrutura de dados para uso da instrucao rdtsc (contador de timestamp em clocks nivel HW)
typedef union {
   unsigned long long int64;
   struct {
      unsigned int lo, hi;
   } int32;
} tsc_counter;

//macro para uso da instrucao rdtsc: RDTSC retorna o TSC (Time Stamp Counter) (numero de ciclos desde o ultimo reset) 
//nos registradores EDX:EAX
#define RDTSC(cpu_c)                 \
  __asm__ __volatile__ ("rdtsc" :    \
  "=a" ((cpu_c).int32.lo),           \
  "=d" ((cpu_c).int32.hi) )
  

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

//funcao main
int main(void) {

   struct timeval inicio, fim;
   tsc_counter tsc1, tsc2;
   long long unsigned int clock;
   double tempo;
   

   float array_a[10] = {1.45,2.98,3.58,4.70,5.60,6.57,7.19,8.40,9.69,10.25};
   float array_p[10] = {1.45,2.98,3.58,4.70,5.60,6.57,7.19,8.40,9.69,10.25};
   int limite = 10; 
   int i;
   
   //mede o tempo de execução da funcao f0 (media de NITER repeticoes) 
   gettimeofday(&inicio, NULL); 
   RDTSC(tsc1);
   
   for(i=1; i<NITER; i++)
       somap1(array_a,array_p,limite);
   
   RDTSC(tsc2);
   gettimeofday(&fim, NULL);
   tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
   clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
   printf("Tempo (somap1): %.1lf(ms) Clocks: %.2e \n", tempo/NITER, (double)clock/NITER);
   printf("Clock/tempo: %.2e\n\n", clock/tempo);

   gettimeofday(&inicio, NULL); 
   RDTSC(tsc1);
         
   for(i=1; i<NITER; i++)
       somap2(array_a,array_p,limite);

   RDTSC(tsc2);
   gettimeofday(&fim, NULL);
   tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
   clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
   printf("Tempo (somap2): %.1lf(ms) Clocks: %.2e \n", tempo/NITER, (double)clock/NITER);
   printf("Clock/tempo: %.2e\n\n", clock/tempo);

 
  return 1;
}
