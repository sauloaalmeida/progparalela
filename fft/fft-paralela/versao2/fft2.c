#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define NUM_ITERACOES 1
#define ISIGN 1

//dados globais, usados pelas threads
int qtdThreads, pesoThreads;
unsigned long qtdElementos, tamArray;
float *data;
pthread_t *threads;
unsigned long mmax,j,m,istep,i;
double wtemp,wr,wpr,wpi,wi,theta;


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

void *fftThread(void *parms){
    int threadId = (int)parms;
    
    printf("threadId:%d\n",threadId);
    sleep(10);
    
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
    
    //inicializando as threads 
    int threadId = 0;
    for (threadId=0; threadId<qtdThreads; threadId++) {
        pthread_create(&threads[threadId], NULL, fftThread, (void*) threadId);
    }
                       
    //finlizando pthread
    for (threadId = 0; threadId < qtdThreads; threadId++) {
        pthread_join(threads[threadId], NULL);
    }
    pthread_exit (NULL);                   
}

int main(int argc, char *argv[]) {
    
    if(argc < 4) {
        printf("ERRO: informe s quantidade de elementos do FFT, a quantidade de threads e o peso das threads:: <./fft2> <qtdElementos> <qtdThreads> <pesoThreads>\n");
        return -1;
    }
    
    //inicializando valores do algoritmo
    qtdElementos = strtoul(argv[1],NULL,10);
    tamArray = qtdElementos*2 + 1;
    qtdThreads = atoi(argv[2]);
    pesoThreads = atoi(argv[3]);
    
    printf("sera processada uma fft com Elementos:%lu, Threads:%d, PesoThread:%d\n",qtdElementos,qtdThreads,pesoThreads);
    
    
    //alocando memoria para o vetor de dados
    data = malloc(sizeof(float) * tamArray);
    
    //alocando memoria para threads
    threads = malloc(sizeof(pthread_t) * qtdThreads);
    
    //repete as execucoes para calcular a media
    unsigned long count;
    for(count=0;count<NUM_ITERACOES;count++){
        
        inicializaArray();
        imprimeVetor();
        
        ordenaBitReverso();
        imprimeVetor();
        
        fft();
    }
    
    //liberando a memoria
    free(threads);
	free(data);
    
    return 0;

}


