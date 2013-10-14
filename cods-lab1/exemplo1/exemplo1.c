/*
Programa: exemplo1.c
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: programa exemplo para uso da ferramenta gprof
*/

#include <stdio.h>
#define DIVISOR 12m

//fatorial sem recursao
long long unsigned int fatorial0(int n) {
   int res=1, i;
   for (i=n; i>0; i--)
      res = res * i;
   return res;
}

//fatorial com recursao
long long unsigned int fatorial1(int n) {
   if (n==0) return 1;
   else return n * fatorial1(n-1);
}

//primeira funcao 'boba', chama fatorial sem recurcao
double f0() {
   int i; double ret=0;
   for(i=1; i<10000000; i++)
      ret += (fatorial0(i%DIVISOR));
   return ret;
}

//segunda funcao 'boba', chama fatorial com recursao
double f1() {
   int i; double ret=0;
   for(i=1; i<10000000; i++)
      ret += (fatorial1(i%DIVISOR));
   return ret;
}

//funcao main
int main(void) {
   printf("resultado de f0 eh %.1lf\n", f0());
   printf("resultado de f1 eh %.1lf\n", f1());
   return 1;
}
