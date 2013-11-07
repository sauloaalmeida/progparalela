/* Prof.: Silvana Rossetto */
/* Laboratório: 3 */
/* Codigo: "Hello World" usando threads em C */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  10

//--funcao executada pelas threads
void *PrintHello (void *threadid) {
  int *tid = (int*) threadid;
  printf("Hello World da thread: %d\n", *tid);
  free(tid);
  pthread_exit(NULL);
}

//--funcao principal do programa
int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];
  int t;
  for(t=0; t<NTHREADS; t++) {
    int *id;
    id = malloc(sizeof(int));
    *id = t;
    printf("--Cria a thread %d\n", t);
    if (pthread_create(&tid[t], NULL, PrintHello, (void*) id)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }
  //--espera todas as threads terminarem
  for (t=0; t<NTHREADS; t++) {
    if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  } 
  printf("--Thread principal terminou\n");
  pthread_exit(NULL);
}
