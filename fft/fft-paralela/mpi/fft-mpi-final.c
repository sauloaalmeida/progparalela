#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define ISIGN 1
#define NUM_ITERACOES 10


typedef struct strDataTransporte
{ 
    unsigned long index;
    float value;
}DataTransporte;

void imprimeVetor(float *dataPrint, unsigned long tamTotalArrayPrint){
    int posicao;
    for(posicao=1;posicao<tamTotalArrayPrint;posicao++){
        printf("%2.2f ",dataPrint[posicao]);
    }
    printf("\n\n");
}

void ordenaBitReverso(float *dataOrdena, unsigned long tamTotalArrayOrdena){
    
    unsigned long i,j,m,qtdElementos;
    float tempr;
    j=1;
    qtdElementos=(tamTotalArrayOrdena-1)/2;
    
    for (i=1;i<tamTotalArrayOrdena;i+=2) {
        if (j > i) {
            SWAP(dataOrdena[j],dataOrdena[i]);
            SWAP(dataOrdena[j+1],dataOrdena[i+1]);
        }
        m=qtdElementos;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
}

void inicializaArray(float *dataInit, unsigned long tamTotalArrayInit){
    
    unsigned long i;
    
    for (i=1; i<tamTotalArrayInit; i+=8) {
        dataInit[i]=0.;
        dataInit[i+1]=0.;
        dataInit[i+2]=1.;
        dataInit[i+3]=0.;
        dataInit[i+4]=2.;
        dataInit[i+5]=0.;
        dataInit[i+6]=3.;
        dataInit[i+7]=0.;
    }
    
}

void calculoButterflyLoopBloco(float data[], int numBloco, unsigned long m,unsigned long mmax, unsigned long istep, double wr, double wi, unsigned long tamBloco){
    
    float tempr,tempi;
    unsigned long j,i,bloco;
    bloco=1;
    
    for (i=m;bloco<=tamBloco;i+=istep) {
        j=i+mmax;                                
        printf("          >>> FOR 2 -> numBloco:%d, wi:%f wr:%f istep:%lu mmax:%lu i:%lu j:%lu\n",numBloco,wi,wr,istep, mmax, i, j);
        tempr=wr*data[j]-wi*data[j+1];
        tempi=wr*data[j+1]+wi*data[j];
        data[j]=data[i]-tempr;
        data[j+1]=data[i+1]-tempi;
        data[i] += tempr;
        data[i+1] += tempi; 
        bloco++;
    }
}

void fft(float *dataFFT, unsigned long tamTotalArrayFFT, int qtdProcessosFFT){
    
    inicializaArray(dataFFT,tamTotalArrayFFT);
    //imprimeVetor(data,tamArrayFft);
    
    ordenaBitReverso(dataFFT,tamTotalArrayFFT);
    //imprimeVetor(data,tamArrayFft);
    
    unsigned long mmax,j,m,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    unsigned long qtdElementos = (tamTotalArrayFFT-1)/2;
    
    
    mmax=2;
    while (qtdElementos >= mmax) {
        //printf(">>> Inicio do while -> mmax:%lu\n",mmax);
        istep=mmax << 1;
        theta=ISIGN*(6.28318530717959/mmax);
        wtemp=sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi=sin(theta);
        wr=1.0;
        wi=0.0;
        for (m=1;m<mmax;m+=2) {
            printf("     >>> FOR 1 -> m:%lu mmax:%lu\n",m,mmax); 
            
            if (qtdElementos/mmax >= qtdProcessosFFT) {
                int blocoAtual;            
                unsigned long tamBloco = qtdElementos/mmax/qtdProcessosFFT;
                DataTransporte *dataTransportePackage = malloc(sizeof(DataTransporte)*4*tamBloco);
                unsigned long indexTransporte = 0;
                
                //loop das threads pela quantidade de cores
                for (blocoAtual=0; blocoAtual<qtdProcessosFFT; blocoAtual++) {
                    //printf("     qtdBlocos:%lu\n",blocoAtual);
                    
                    unsigned long i,j;
                    unsigned long m = (((qtdElementos*2)/qtdProcessosFFT)*blocoAtual)+m;
                    
                    for (i=m;indexTransporte<=tamBloco;i+=istep) {
                        j=i+mmax;                                
                        printf("          >>> FOR 2 -> numBloco:%d, wi:%f wr:%f istep:%lu mmax:%lu i:%lu j:%lu\n",numBloco,wi,wr,istep, mmax, i, j);
                        
                        dataTransportePackage[indexTransporte].index=i;
                        dataTransportePackage[indexTransporte].value=dataFFT[i];
                        
                        dataTransportePackage[indexTransporte+1].index=j;
                        dataTransportePackage[indexTransporte+1].value=dataFFT[j];

                        dataTransportePackage[indexTransporte+2].index=i+1;
                        dataTransportePackage[indexTransporte+2].value=dataFFT[i+1];
                        
                        dataTransportePackage[indexTransporte+3].index=j+1;
                        dataTransportePackage[indexTransporte+3].value=dataFFT[j+1];                        
                        
                        indexTransporte+=4;
                    }

                                        
                    calculoButterflyLoopBlocoMPI(dataTransportePackage,wr,wi,tamBloco);
                    //calculoButterflyBloco(m,mmax,istep,wr,wi,QTD_ELEMENTOS/mmax);
                    

                    
                }
                
                free(dataTransportePackage);
                
            } else {
                printf("     processa na main\n");
                calculoButterflyLoopBloco(dataFFT,0,m,mmax,istep,wr,wi,qtdElementos/mmax);
            }
            
            wr=(wtemp=wr)*wpr-wi*wpi+wr;
            wi=wi*wpr+wtemp*wpi+wi;
        }
        mmax=istep;
        //printf("<<< Final do while -> istep:%lu mmax:%lu\n\n",istep,mmax);
    }
    
}


int main(int argc, char *argv[]) {
    
    unsigned long tamTotalArray, qtdElementos, qtdElementosArray;
    int qtdProcessos;
    float *data;
    
    
    if(argc < 3) {
        printf("ERRO: informe s quantidade de elementos do FFT, a quantidade de threads : <./fft-mpi> <qtdElementos> <qtdProcessos> \n");
        return -1;
    }
    
    //inicializando valores do algoritmo
    qtdElementos = strtoul(argv[1],NULL,10);
    qtdElementosArray = qtdElementos*2;
    tamTotalArray = qtdElementosArray + 1;
    qtdProcessos = atoi(argv[2]);
    
    //printf("qtdElementos=%lu,tamArray=%lu,qtdProcessos=%d\n",qtdElementos,tamArray,qtdProcessos);
    
    data = malloc(sizeof(float)*tamTotalArray);
    
    fft(data,tamTotalArray,qtdProcessos);
    
    imprimeVetor(data,tamTotalArray);
    
    free(data);
    
}
