/*
Programa: exemplo4.c
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: programa exemplo para ilustrar limitações para transformar código de forma segura (chamadas de funcoes)
*/

#include <stdio.h>

//funcao auxiliar
int f();

//função 1
int f1() {
   return f()+f()+f()+f();
}

//função 2
int f2() {
   return 4*f();
}

//variável global
int g_contador;

int f() {
   return g_contador++;
}

//funcao main
int main(void) {
   g_contador = 0;
   printf("O resultado de f1 é: %d\n", f1()); 
   g_contador = 0;
   printf("O resultado de f2 é: %d\n", f2()); 
   return 1;
}
