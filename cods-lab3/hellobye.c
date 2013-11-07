/* Prof.: Silvana Rossetto */
/* Codigo: Uso de variáveis de condição e suas operações básicas para sincronização por condição */

/***** Condicao logica da aplicacao: a thread B so pode imprimir "ByeBye" depois que duas threads A imprimirem  "Hello"  ****/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  3

/* Variaveis globais */
int x = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

/* Thread A */
void *A (void *t) {
  int i; int my_id = (int)t;
  int boba1, boba2;

  printf("A: Comecei: thread %d\n", my_id);
  
  /* faz alguma coisa inutil pra gastar tempo... */
  boba1=10000; boba2=-10000; while (boba2 < boba1) boba2++;
  
  printf("HELLO\n");

  pthread_mutex_lock(&x_mutex);
  x++;
  if (x==2) {
      printf("A: thread %d, x = %d, vai sinalizar a condicao \n", my_id, x);
      pthread_cond_signal(&x_cond);
  }
  pthread_mutex_unlock(&x_mutex);

  pthread_exit(NULL);
}

/* Thread B */
void *B (void *t) {
  int my_id = (int)t;

  printf("B: Comecei: thread %d\n", my_id);

  pthread_mutex_lock(&x_mutex);
  if (x < 2) { 
     printf("B: thread %d x= %d, vai se bloquear...\n", my_id, x);
     pthread_cond_wait(&x_cond, &x_mutex);
     printf("B: thread %d, sinal recebido e mutex realocado, x = %d\n", my_id, x);
  }
  printf("BYE BYE\n");
  pthread_mutex_unlock(&x_mutex); 
  pthread_exit(NULL);
}


/* Funcao principal */
int main(int argc, char *argv[]) {
  int i; 
  int t1=1, t2=2, t3=3;
  pthread_t threads[3];

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);

  /* Cria as threads */
  pthread_create(&threads[0], NULL, A, (void *)t1);
  pthread_create(&threads[1], NULL, A, (void *)t2);
  pthread_create(&threads[2], NULL, B, (void *)t3);

  /* Espera todas as threads completarem */
  for (i = 0; i < NTHREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  printf ("\nFIM.\n");

  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);
  pthread_exit (NULL);
}
