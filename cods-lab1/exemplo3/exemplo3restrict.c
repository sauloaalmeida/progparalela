/*
Programa: exemplo3.c
Disciplina: Ambientes de Programacao Paralela (PPGI-2013/3)
Descrição: programa exemplo para ilustrar uso do qualificador "restrict" (de C99) para reduzir limitações para transformar código de forma segura
*/

#include <stdio.h>

//função 1
void f1(int *restrict xp, int *restrict yp) {
   *xp = *xp + *yp;
   *xp = *xp + *yp;
}

//função 2
void f2(int *restrict xp, int *restrict yp) {
   *xp = *xp + (2 * (*yp));
}


//funcao main
int main(void) {
   int x, y;
   int *restrict xp, *restrict yp;

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
   //ATENCAO: o programador deveria ser responsavel por nao fazer essa nova atribuicao para os ponteiros, o compilador nao verifica!!
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
