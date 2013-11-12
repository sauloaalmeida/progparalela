#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define QTD_ELEMENTOS 16
#define TAM_ARRAY QTD_ELEMENTOS * 2 + 1
#define QTD_ELEMENTOS_ARRAY TAM_ARRAY - 1
#define ISIGN 1
#define QTD_CORES 2 
#define NUM_ITERACOES 1
//#define NUM_ITERACOES 1000000000

//dados globais usados pelas threads
int barreira = QTD_CORES;
float data[TAM_ARRAY];


void imprimeVetor(){
	int posicao;
	for(posicao=1;posicao<TAM_ARRAY;posicao++){
	       printf("%2.2f ",data[posicao]);
	}
    printf("\n\n");
}

void ordenaBitReverso(){
   
   unsigned long i,j,m;
   float tempr;
   j=1;

   for (i=1;i<TAM_ARRAY;i+=2) {
      if (j > i) {
           SWAP(data[j],data[i]);
           SWAP(data[j+1],data[i+1]);
       }
       m=QTD_ELEMENTOS;
       while (m >= 2 && j > m) {
           j -= m;
           m >>= 1;
       }
       j += m;
   }
}

void calculoButterflyLoopBloco(unsigned long m,unsigned long mmax, unsigned long istep, double wr, double wi, unsigned long tamBloco){
	
    float tempr,tempi;
    unsigned long j,i,bloco;
    bloco=1;
    
    for (i=m;bloco<=tamBloco;i+=istep) {
        j=i+mmax;				
        printf("          >>> FOR 2 -> wi:%f wr:%f istep:%lu mmax:%lu i:%lu j:%lu\n",wi,wr,istep, mmax, i, j);
        tempr=wr*data[j]-wi*data[j+1];
        tempi=wr*data[j+1]+wi*data[j];
        data[j]=data[i]-tempr;
        data[j+1]=data[i+1]-tempi;
        data[i] += tempr;
        data[i+1] += tempi; 
        bloco++;
    }
}


void fft(){

	unsigned long mmax,j,m,istep,i;
     double wtemp,wr,wpr,wpi,wi,theta;
    
     //inicializa as threads pela quantidade de cores
     pthread_t threads[QTD_CORES];

	mmax=2;
	while (TAM_ARRAY > mmax) {
	printf(">>> Inicio do while -> mmax:%lu\n",mmax);
		istep=mmax << 1;
		theta=ISIGN*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
		printf("     >>> FOR 1 -> m:%lu mmax:%lu\n",m,mmax); 
            
            
            if (QTD_CORES > 1 && QTD_ELEMENTOS/mmax >= QTD_CORES) {
                printf("     processa em thread\n");
                unsigned long tamBloco = QTD_ELEMENTOS/mmax/QTD_CORES;
                unsigned long blocoAtual;
                
                //loop das threads pela quantidade de cores
                for (blocoAtual=0; blocoAtual<QTD_CORES; blocoAtual++) {
                    printf("     qtdBlocos:%lu\n",blocoAtual);
                    calculoButterflyLoopBloco((((QTD_ELEMENTOS_ARRAY)/QTD_CORES)*blocoAtual)+m,mmax,istep,wr,wi,tamBloco);
                }
                
            } else {
                printf("     processa na main\n");
                calculoButterflyLoopBloco(m,mmax,istep,wr,wi,QTD_ELEMENTOS/mmax);
            }
            
            //--espera todas as threads terminarem
            /*int t=0;
            for (t=0; t<QTD_CORES; t++) {
                if (pthread_join(threads[t], NULL)) {
                    printf("--ERRO: pthread_join() \n"); exit(-1); 
                } 
            } */

			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
        printf("<<< Final do while -> istep:%lu mmax:%lu\n\n",istep,mmax);
	}
}

void inicializaArray(){
    
    unsigned long i;
    
    for (i=1; i<TAM_ARRAY; i+=8) {
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

int main(void) {

     unsigned long count;

	for(count=0;count<NUM_ITERACOES;count++){
     	inicializaArray();
          imprimeVetor();

          ordenaBitReverso();
          imprimeVetor();
		
          fft();
     	imprimeVetor();
	}





	return 0;

}


