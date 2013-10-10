/*
Programa: exemplo2.c
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: programa exemplo para tomada de tempo medio de execução e numero total de clocks
*/

#include <stdio.h>
#include <sys/time.h>
#define NITER 10
#define DIVISOR 12

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

//fatorial sem recursao
int fatorial0(int n) {
   int res=1, i; 
   for (i=n; i>0; i--)
      res = res * i;
   return res;
}

//fatorial com recursao
int fatorial1(int n) {
   if (n==0) return 1; 
   else return n * fatorial1(n-1);
}

//primeira funcao boba, chamafatorial sem recursao
double f0() {
   int i; double ret=0;
   for(i=1; i<10000000; i++)
      ret += 1/(fatorial0(i%DIVISOR));
   return ret;
}

//segunda funcao boba, chama fatorial com recursao
double f1() {
   int i; double ret=0;
   for(i=1; i<10000000; i++)
      ret += 1/(fatorial1(i%DIVISOR));
   return ret;
} 

//funcao main
int main(void) {
   struct timeval inicio, fim;
   tsc_counter tsc1, tsc2;
   long long unsigned int clock;
   double tempo;
   int j;
   double res;

   //mede o tempo de execução da funcao f0 (media de NITER repeticoes) 
   gettimeofday(&inicio, NULL); 
   RDTSC(tsc1);
   for (j=0; j<NITER; j++) {
      res = f0();
   }
   RDTSC(tsc2);
   gettimeofday(&fim, NULL);
   tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
   clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
   printf("Resultado e f0: %.1lf \n", res);
   printf("Tempo (fatorial sem recursao): %.1lf(ms) Clocks: %.2e \n", tempo/NITER, (double)clock/NITER);
   printf("Clock/tempo: %.2e\n\n", clock/tempo);
 

   //mede o tempo de execução da funcao f1 (media de NITER repeticoes)
   gettimeofday(&inicio, NULL); 
   RDTSC(tsc1);
   for (j=0; j<NITER; j++) {
      res = f1();
   }
   RDTSC(tsc2);
   gettimeofday(&fim, NULL);
   tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
   clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
   printf("Resultado de f1: %.1lf \n", res);
   printf("Tempo (fatorial com recursao): %.1lf(ms) Clocks: %.2e \n", tempo/NITER, (double)clock/NITER);
   printf("Clock/tempo: %.2e\n\n", clock/tempo);

   return 1;
}
