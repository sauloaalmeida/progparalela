#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <math.h>
#include <sys/time.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define NUM_ITERACOES 5
#define ISIGN 1

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


//dados globais, usados pelas threads
int qtdThreads, pesoThreads,barreira;
unsigned long qtdElementos, tamArray;
float *data;
pthread_t *threads;
unsigned long mmax,j,m,istep,i;
double wtemp,wr,wpr,wpi,wi,theta;

int barreira;
pthread_mutex_t barreiraMutex;
pthread_cond_t barreiraCond;

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


void calculoButterflyBloco(unsigned long posInicial, unsigned long tamBloco){
    
    float tempr,tempi;
    unsigned long j,i,bloco;
    bloco=1;
    
    
    for (i=posInicial;bloco<=tamBloco;i+=istep) {
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
    int threadId = (int)parms;
    unsigned long tamLoop, tamBloco;
    
    while (qtdElementos >= mmax) {
            
            //calcula o tamanho do loop e do bloco para a execucao
            tamLoop = qtdElementos/mmax; 
            tamBloco = tamLoop/qtdThreads;
        
            
            //se for para processar em bloco 
            if(qtdThreads > 1 && tamLoop >= (pow(qtdThreads,pesoThreads)) ){
                
                //printf("          processar em um bloco da thread: %d\n",threadId);
                calculoButterflyBloco((((tamArray-1)/qtdThreads)*threadId)+m, tamBloco);
                
                
           
            }else if ( (qtdThreads == 1 || tamLoop < (pow(qtdThreads,pesoThreads)) ) && threadId == 0 ){ 
                //se for apenas um core, ou se for menor que a quantidade de threads com peso
                //e se for a ultima a thread da barreira, deixa ela processar sozinha
                
                //printf("          processar em uma unica thread\n");
                calculoButterflyBloco(m, tamLoop);
                
            }                                  
            else{
            
                //printf("          thread: %d nao faz nada\n",threadId);
                
            }
            //obtem o lock
            pthread_mutex_lock(&barreiraMutex);            
            //incrementa a barreira
            barreira++;
        
            //se for a ultima thread da barreira
            if(barreira==qtdThreads){
                
                //se for a ultima da barreira
                //atualiza os valores do For1 
                wr=(wtemp=wr)*wpr-wi*wpi+wr;
                wi=wi*wpr+wtemp*wpi+wi;
                m+=2;
                //printf("     atualiza os valores do For1 m:%lu mmax:%lu istep:%lu\n",m,mmax,istep);
                //se necessario atualiza os valores do While
                if (m>=mmax) {                    
                    //printf("atualizando dados dos while, pela thread :%d, barreira:%d \n",threadId, barreira);
                    //atualiza o mmax
                    mmax=istep;
                    //atualiza os valores do while para a proxima execucao
                    istep=mmax << 1;
                    theta=ISIGN*(6.28318530717959/mmax);
                    wtemp=sin(0.5*theta);
                    wpr = -2.0*wtemp*wtemp;
                    wpi=sin(theta);
                    wr=1.0;
                    wi=0.0;
                    m=1;
                }
                
                //inicializa a barreira
                barreira=0;
                //acorda as outras threads
                pthread_cond_broadcast(&barreiraCond);                

                
            }else{ 
                //se nao for a ultima thread da barreira se coloca para dormir
                //printf("     colocando a threadId:%d para dormir\n",threadId);
                pthread_cond_wait(&barreiraCond, &barreiraMutex); 
            }
        
        pthread_mutex_unlock(&barreiraMutex);
         
    }
    //printf("Finalizando threadId:%d\n",threadId);
    pthread_exit(NULL);
}


void fft(){
    //inicializando os dados do algoritmo
    mmax=2;
    istep=mmax << 1;
    theta=ISIGN*(6.28318530717959/mmax);
    wtemp=sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi=sin(theta);
    wr=1.0;
    wi=0.0;
    m=1;
    barreira=0;
    
    /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
    pthread_mutex_init(&barreiraMutex, NULL);
    pthread_cond_init (&barreiraCond, NULL);
    
    //inicializando as threads 
    int threadId = 0;
    for (threadId=0; threadId<qtdThreads; threadId++) {
        pthread_create(&threads[threadId], NULL, fftThread, (void*) threadId);
    }
    
    //finalizando pthread
    for (threadId = 0; threadId < qtdThreads; threadId++) {
        pthread_join(threads[threadId], NULL);
    }
    //imprimeVetor();
    pthread_mutex_destroy(&barreiraMutex);
    pthread_cond_destroy(&barreiraCond);
    //pthread_exit (NULL);
}

int main(int argc, char *argv[]) {
    
    struct timeval inicio, fim;
    tsc_counter tsc1, tsc2;
    long long unsigned int clock;
    double tempo;

    
    if(argc < 4) {
        printf("ERRO: informe s quantidade de elementos do FFT, a quantidade de threads e o peso das threads:: <./fft2> <qtdElementos> <qtdThreads> <pesoThreads>\n");
        return -1;
    }
    
    //inicializando valores do algoritmo
    qtdElementos = strtoul(argv[1],NULL,10);
    tamArray = qtdElementos*2 + 1;
    qtdThreads = atoi(argv[2]);
    pesoThreads = atoi(argv[3]);
    
    //printf("sera processada uma fft com Elementos:%lu, Threads:%d, PesoThread:%d\n",qtdElementos,qtdThreads,pesoThreads);
    
    
    //alocando memoria para o vetor de dados
    data = malloc(sizeof(float) * tamArray);
    
    //alocando memoria para threads
    threads = malloc(sizeof(pthread_t) * qtdThreads);
    
    
    //warmup
    inicializaArray();
    ordenaBitReverso();
    fft();
    
    //repete as execucoes para calcular a media
    unsigned long count;
    
    //mede o tempo de execução da funcao f0 (media de NITER repeticoes) 
    gettimeofday(&inicio, NULL);
    RDTSC(tsc1);
    for(count=0;count<NUM_ITERACOES;count++){
        
        inicializaArray();
        //imprimeVetor();
        
        ordenaBitReverso();
        //imprimeVetor();
        
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
    free(threads);
	free(data);
    
    pthread_exit (NULL);
    return 0;

}


