/*
Programa: exemplo3.c
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: programa exemplo para ilustrar limitações para transformar código de forma segura (aliasing de ponteiros)
*/

#include <stdio.h>

//função 1
void f1(int *xp, int *yp) {
   *xp = *xp + *yp;
   *xp = *xp + *yp;
}

//função 2
void f2(int *xp, int *yp) {
   *xp = *xp + (2 * (*yp));
}


//funcao main
int main(void) {
   int x, y;
   int *xp, *yp;

   //inicializa os ponteiros
   xp = &x;
   yp = &y;
   printf("Para xp != yp:\n");
   //inicializa os conteúdos das variáveis e chama f1
   x=5; y=4;
   f1(xp, yp);
   printf("Apos f1, x=%d\n", x);
   //reinicializa os conteúdos das variáveis e chama f2
   x=5; y=4;
   f2(xp, yp);
   printf("Apos f2, x=%d\n", x);

   
   //reinicializa os ponteiros
   xp = &x;
   yp = &x;
   printf("Para xp == yp:\n");
   //inicializa os conteúdos das variáveis e chama f1
   x=5;
   f1(xp, yp);
   printf("Apos f1, x=%d\n", x);
   //reinicializa os conteúdos das variáveis e chama f2
   x=5;
   f2(xp, yp);
   printf("Apos f2, x=%d\n", x);


   return 1;
}
