/* Prof.: Silvana Rossetto */
/* Laboratório: 3 */
/* Codigo: Comunicação entre threads usando variável compartilhada, e solucao de Peterson */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2

volatile short int queroEntrar[2];
volatile short int turno;

volatile int s = 0; //variavel compartilhada entre as threads

//funcao executada pelas threads
void *ExecutaTarefa (void *threadid) {
  int i, fim;
  int tid = (int) threadid;
  fim=10000000;
  printf("Thread : %d esta executando...\n", (int) tid);
  for (i=0; i<fim; i++) {
     //--entrada na SC
     queroEntrar[tid]=1;
     turno = (tid+1)%2;
     //printf("Thread : %d quer entrar na SC...\n", (int) tid);
     while(queroEntrar[(tid+1)%2] && turno==(tid+1)%2) {;}
     //--SC
     //printf("Thread : %d esta na SC...\n", (int) tid);
     s++; //incrementa o contador de tarefas realizadas 
     //--saida da SC
     //printf("Thread : %d saiu da SC...\n", (int) tid);
     queroEntrar[tid]=0;
  }
  printf("Thread : %d terminou!\n", (int) tid);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];
  int t;

  queroEntrar[0]=0;
  queroEntrar[1]=0;

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
