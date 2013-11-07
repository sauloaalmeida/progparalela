#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define QTD_ELEMENTOS 4
#define TAM_ARRAY 8 
#define ISIGN 1
#define QTD_CORES 2


void imprimeVetor(float dadosImp[]){
	int posicao;
	for(posicao=0;posicao<TAM_ARRAY;posicao++){
	       printf("%2.2f ",dadosImp[posicao]);
	}
    printf("\n\n");
}

void ordenaBitReverso(float data[]){
   
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

void calculoButterfly(float data[], unsigned long i, unsigned long j, double wr, double wi){
	
	float tempr,tempi;

	tempr=wr*data[j]-wi*data[j+1];
        tempi=wr*data[j+1]+wi*data[j];
	data[j]=data[i]-tempr;
	data[j+1]=data[i+1]-tempi;
	data[i] += tempr;
	data[i+1] += tempi;
}

void fft(float data[]){

	unsigned long mmax,j,m,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;

	mmax=2;
	while (TAM_ARRAY > mmax) {

		istep=mmax << 1;
		theta=ISIGN*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=TAM_ARRAY;i+=istep) {
				j=i+mmax;
				calculoButterfly(data,i,j,wr,wi);
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

int main(void) {

		float data[TAM_ARRAY] = {0.,0.,1.,0.,2.,0.,3.,0.};

		ordenaBitReverso(data-1);

		fft(data-1);

		imprimeVetor(data);

		return 0;

}


