/* Prof.: Silvana Rossetto */
/* Laboratório: 3 */
/* Codigo: Comunicação entre threads usando variável compartilhada */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2

volatile int s = 0; //variavel compartilhada entre as threads

//funcao executada pelas threads
void *ExecutaTarefa (void *threadid) {
  int i, fim;
  int tid = (int) threadid;
  fim=10000000;
  printf("Thread : %d esta executando...\n", tid);
  for (i=0; i<fim; i++) {
     //recebe alguma tarefa e a executa...
     s++; //incrementa o contador de tarefas realizadas 
  }
  printf("Thread : %d terminou!\n", tid);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];
  int t;
  for(t=0; t<NTHREADS; t++) {
    printf("--Cria a thread %d\n", t);
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }
  //--espera todas as threads terminarem
  for (t=0; t<NTHREADS; t++) {
    if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  } 
  printf("Valor de s = %d\n", s);
  pthread_exit(NULL);
}
