/*
Biblioteca: matriz.h
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: implementa funcoes para manipulacao de matrizes e tomada de tempo
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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

//funcao que inicializa uma matriz com 0s
int inicMatriz(double *mat, int linhas, int colunas);

//funcao que carrega uma matriz
int carregaMatriz(double *mat, FILE *fp, int linhas, int colunas);

//funcao que imprime uma matriz
int imprimeMatriz(double *mat, int linhas, int colunas);
