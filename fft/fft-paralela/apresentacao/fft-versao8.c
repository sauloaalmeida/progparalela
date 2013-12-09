#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define ISIGN 1
#define NUM_ITERACOES 10

//dados globais usados pelas threads
typedef struct strDadosThread
{
    unsigned long id;
    unsigned long m;
    unsigned long mmax;
    unsigned long istep;
    double wr;
    double wi; 
    unsigned long tamBloco;
}DadosThread;

unsigned long qtdElementos, tamArray;
int qtdThreads, pesoThreads;
float *data;
pthread_t *threads;
DadosThread *dadosThreads;


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

void imprimeVetor(){
        int posicao;
        for(posicao=1;posicao<tamArray;posicao++){
               printf("%2.2f ",data[posicao]);
        }
    printf("\n\n");
}

void ordenaBitReverso(){
    
    unsigned long i,j,m;
    float tempr;
    j=1;
    
    for (i=1;i<tamArray;i+=2) {
        if (j > i) {
            SWAP(data[j],data[i]);
            SWAP(data[j+1],data[i+1]);
        }
        m=qtdElementos;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
}

void inicializaArray(){
    
    unsigned long i;
    
    for (i=1; i<tamArray; i+=8) {
        data[i]=0.;
        data[i+1]=0.;
        data[i+2]=1.;
        data[i+3]=0.;
        data[i+4]=2.;
        data[i+5]=0.;
        data[i+6]=3.;
        data[i+7]=0.;
    }
    
}



void calculoButterflyBloco(unsigned long m,unsigned long mmax, unsigned long istep, double wr, double wi, unsigned long tamBloco){
        
    float tempr,tempi;
    unsigned long j,i,bloco;
    bloco=1;
    
    for (i=m;bloco<=tamBloco;i+=istep) {
        j=i+mmax;                                
        //printf("          >>> FOR 2 -> wi:%f wr:%f istep:%lu mmax:%lu i:%lu j:%lu\n",wi,wr,istep, mmax, i, j);
        tempr=wr*data[j]-wi*data[j+1];
        tempi=wr*data[j+1]+wi*data[j];
        data[j]=data[i]-tempr;
        data[j+1]=data[i+1]-tempi;
        data[i] += tempr;
        data[i+1] += tempi; 
        bloco++;
    }
}

void *fftThread(void *parms){


     DadosThread *dadosThread;
     dadosThread = (DadosThread *) parms;

     //printf("Thread:%lu Entered. m:%lu mmax:%lu istep:%lu wr:%f wi:%f tamBloco:%lu \n", dadosThread->id,dadosThread->m,dadosThread->mmax,dadosThread->istep,dadosThread->wr,dadosThread->wi,dadosThread->tamBloco);
        
        calculoButterflyBloco(dadosThread->m,dadosThread->mmax,dadosThread->istep,dadosThread->wr,dadosThread->wi,dadosThread->tamBloco);

          /*
        printf("Thread:%d Working\n",id);
        sleep(15);
        printf("Thread %d Done with work\n",id);*/

   pthread_exit(NULL);

}


void fft(){

        unsigned long mmax,j,m,istep,i;
     double wtemp,wr,wpr,wpi,wi,theta;
    
     //inicializa as threads pela quantidade de cores
     pthread_t threads[qtdThreads];

        mmax=2;
        while (tamArray > mmax) {
        //printf(">>> Inicio do while -> mmax:%lu\n",mmax);
                istep=mmax << 1;
                theta=ISIGN*(6.28318530717959/mmax);
                wtemp=sin(0.5*theta);
                wpr = -2.0*wtemp*wtemp;
                wpi=sin(theta);
                wr=1.0;
                wi=0.0;
                for (m=1;m<mmax;m+=2) {
                //printf("     >>> FOR 1 -> m:%lu mmax:%lu\n",m,mmax); 
            

                    unsigned long blocoAtual;            
                    if (qtdThreads > 1 && qtdElementos/mmax >= (pow(qtdThreads,pesoThreads))) {
                        //printf("     processa em thread\n");
                        unsigned long tamBloco = qtdElementos/mmax/qtdThreads;
                
                        //loop das threads pela quantidade de cores
                        for (blocoAtual=0; blocoAtual<qtdThreads; blocoAtual++) {
                            //printf("     qtdBlocos:%lu\n",blocoAtual);
                    
                            //preeenche os dados para enviar para a thread
                            dadosThreads[blocoAtual].id = blocoAtual;
                            dadosThreads[blocoAtual].m = (((tamArray - 1)/qtdThreads)*blocoAtual)+m;
                            dadosThreads[blocoAtual].mmax = mmax;
                            dadosThreads[blocoAtual].istep = istep;
                            dadosThreads[blocoAtual].wr = wr;
                            dadosThreads[blocoAtual].wi = wi; 
                            dadosThreads[blocoAtual].tamBloco = tamBloco;
                
                            //calculoButterflyLoopBloco((((qtdElementos_ARRAY)/qtdThreads)*blocoAtual)+m,mmax,istep,wr,wi,tamBloco);
                                pthread_create(&threads[blocoAtual], NULL, fftThread, (void*) &dadosThreads[blocoAtual]);
                        }

                //--espera todas as threads terminarem
               for (blocoAtual=0; blocoAtual<qtdThreads; blocoAtual++) {
                   if (pthread_join(threads[blocoAtual], NULL)) {
                        //printf("--ERRO: pthread_join() \n"); exit(-1);
                   }
                } 
                
            } else {
                //printf("     processa na main\n");
                calculoButterflyBloco(m,mmax,istep,wr,wi,qtdElementos/mmax);
            }

                  wr=(wtemp=wr)*wpr-wi*wpi+wr;
                  wi=wi*wpr+wtemp*wpi+wi;
                }
                mmax=istep;
        //printf("<<< Final do while -> istep:%lu mmax:%lu\n\n",istep,mmax);
        }
}


int main(int argc, char *argv[]) {

     struct timeval inicio, fim;
     tsc_counter tsc1, tsc2;
     long long unsigned int clock;
     double tempo;
        unsigned long count;
    
    if(argc < 4) {
        printf("ERRO: informe s quantidade de elementos do FFT, a quantidade de threads e o peso das threads:: <./fft2> <qtdElementos> <qtdThreads> <pesoThreads>\n");
        return -1;
    }
    
    //inicializando valores do algoritmo
    qtdElementos = strtoul(argv[1],NULL,10);
    tamArray = qtdElementos*2 + 1;
    qtdThreads = atoi(argv[2]);
    pesoThreads = atoi(argv[3]);

    //alocando memoria para o vetor de dados
    data = malloc(sizeof(float) * tamArray);
    
    //alocando memoria para threads
    threads = malloc(sizeof(pthread_t) * qtdThreads);
    
    //alocando a memoria para os dados das threads
    dadosThreads = malloc(sizeof(DadosThread)* qtdThreads);

    
     //wurmup
     inicializaArray();
      ordenaBitReverso();
     fft();

     //mede o tempo de execução da funcao f0 (media de NITER repeticoes) 
     gettimeofday(&inicio, NULL);
     RDTSC(tsc1);
        for(count=0;count<NUM_ITERACOES;count++){
             inicializaArray();
             ordenaBitReverso();
             fft();
        }

     RDTSC(tsc2);
     gettimeofday(&fim, NULL);
     tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
     //printf("tempo:%lf",tempo);
     clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
     //printf("Tempo FFT : %.1lf(ms) Clocks: %.2e \n", tempo/NUM_ITERACOES, (double)clock/NUM_ITERACOES);
     //printf("Clock/tempo: %.2e\n", clock/tempo);
     //printf("QtdElementos:%lu Threads: %d Peso:%d\n",qtdElementos,qtdThreads,pesoThreads);
      printf("%lu\t%d\t%d\t%.1lf\t%.2e\t%.2e\n",qtdElementos,qtdThreads,pesoThreads,tempo/NUM_ITERACOES,(double)clock/NUM_ITERACOES,clock/tempo);
        //imprimeVetor();
    
    //liberando a memoria
    free(dadosThreads);
    free(threads);
	free(data);

     pthread_exit (NULL);
        return 0;

}
