/* Prof.: Silvana Rossetto */
/* Laboratório: 3 */
/* Codigo: Comunicação entre threads usando variável compartilhada e sincronização com locks */
// Exemplo de uso de locks recursivos

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS  2

//--variaveis compartilhadas/globais
int s = 0;    //variavel compartilhada entre as threads
pthread_mutex_t mutex;   // lock para exclusao mutua entre as threads

//--funcao de acesso exclusivo
void F() {
  pthread_mutex_lock(&mutex); //entrada na secao critica
  s++;  
  pthread_mutex_unlock(&mutex); //saida da secao critica
}

//--funcao de acesso exclusivo
void G() {
  pthread_mutex_lock(&mutex); //entrada na secao critica
  F();  
  pthread_mutex_unlock(&mutex); //saida da secao critica
}

//--funcao executada pelas threads criadas no programa
//a exclusao mutua eh implementada nas funcoes chamadas
void *ExecutaTarefa (void *threadid) {
  int i;
  int tid = (int) threadid;
  printf("Thread : %d esta executando...\n", (int) tid);
  for (i=0; i<10000000; i++) {
     G();
  }
  printf("Thread : %d terminou!\n", (int) tid);
  pthread_exit(NULL);
}

//--funcao/thread principal do programa
int main(int argc, char *argv[]) {
  pthread_t tid[NTHREADS];
  int t;
  pthread_mutexattr_t mutexAttr;
  
  //--faz o mutex (lock) ser recursivo
  if (pthread_mutexattr_init(&mutexAttr))
      { printf("--ERRO: pthread_mutexattr_init()\n"); exit(-1); }
  if (pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE))
      { printf("--ERRO: pthread_mutexattr_settype()\n"); exit(-1); }
  
  //--inicia o mutex (lock) recursivo
  //if (pthread_mutex_init(&mutex, NULL))
  if (pthread_mutex_init(&mutex, &mutexAttr))
      { printf("--ERRO: pthread_mutex_init()\n"); exit(-1); }
  
  //--cria as threads
  for(t=0; t<NTHREADS; t++) {
    printf("--Cria a thread %d\n", t);
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }
  //--espera todas as threads terminarem para exibir o valor final de s
  for (t=0; t<NTHREADS; t++) {
    if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  } 
  printf("Valor de s = %d\n", s);
  pthread_exit(NULL);
}
