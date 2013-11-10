#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define QTD_ELEMENTOS 8
#define TAM_ARRAY 16
#define QTD_CORES 2

struct fftParam{
      unsigned long nn; 
      int isign;
      float data[TAM_ARRAY]={};
   };
        

void imprimeVetor(float dadosImp[], int tamVet){
	int posicao;
	for(posicao=0;posicao<tamVet;posicao++){
	       printf("%2.2f ",dadosImp[posicao]);
	}
    printf("\n\n");
}

void *fft(void *t){

    struct fftParam fftParamValues = (struct fftParam*)t;

    float data[], unsigned long nn, int isign

	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;

	float tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=nn;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}

	mmax=2;
	while (n > mmax) {

		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);

		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

void carregaDataBloco1(float data[], float newData[]){
		int x;
		
		for(x=0;x<TAM_ARRAY/2;x++){
			newData[x]=data[x];
		}
}

void carregaDataBloco2(float data[], float newData[]){
		int x;
		
		for(x=TAM_ARRAY/2;x<TAM_ARRAY;x++){
			newData[x]=data[x];
		}
}

void totalizaResultado(float vetor1[], float vetor2[], float result[]){
		int x;
		for(x=0;x<TAM_ARRAY;x++){
			result[x]=vetor1[x]+vetor2[x];
		}

}

int main(void) {

pthread_t threads[QTD_CORES];
        
    	float original[TAM_ARRAY] = {0.,0.,1.,0.,2.,0.,3.,0.,0.,0.,1.,0.,2.,0.,3.,0.};
        float resultado[TAM_ARRAY];

        float data1[TAM_ARRAY]={};
		float data2[TAM_ARRAY]={};
		
		carregaDataBloco1(original,struc1);
		carregaDataBloco2(original,struc2);
		
		//imprimeVetor(data1,TAM_ARRAY);
		//imprimeVetor(data2,TAM_ARRAY);
		
        fft(data1-1,QTD_ELEMENTOS,1);
        imprimeVetor(data1,TAM_ARRAY);

		fft(data2-1,QTD_ELEMENTOS,1);
        imprimeVetor(data2,TAM_ARRAY);

		totalizaResultado(data1,data2,resultado);
		imprimeVetor(resultado,TAM_ARRAY);

		fft(original-1,QTD_ELEMENTOS,1);
		imprimeVetor(original,TAM_ARRAY);
		
		pthread_exit (NULL);
		
		return 0;

}


