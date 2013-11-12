#include <stdio.h>
#include <math.h>
#define NITER 100000
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define QTD_ELEMENTOS 16
#define QTD_ELEMENTOS_ARRAY QTD_ELEMENTOS * 2
#define TAM_ARRAY QTD_ELEMENTOS_ARRAY + 1
#define ISIGN 1

float data[TAM_ARRAY];

void imprimeVetor(){
	int posicao;
	for(posicao=1;posicao<TAM_ARRAY;posicao++){
	       printf("%2.2f ",data[posicao]);
	}
     printf("\n\n");
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

void calculoButterfly(float data[], unsigned long i, unsigned long j, double wr, double wi){

  float tempr,tempi;

  tempr=wr*data[j]-wi*data[j+1];
  tempi=wr*data[j+1]+wi*data[j];
  data[j]=data[i]-tempr;
  data[j+1]=data[i+1]-tempi;
  data[i] += tempr;
  data[i+1] += tempi;
}


void fft(){

	unsigned long mmax,j,m,istep,i;
     double wtemp,wr,wpr,wpi,wi,theta;

	mmax=2;

     while (QTD_ELEMENTOS_ARRAY > mmax) {	
		istep=mmax << 1;
		theta=ISIGN*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
               for (i=m;i<TAM_ARRAY;i+=istep) {
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

     int j=1;

     for (j=0; j<NITER; j++) {
          inicializaArray();
          ordenaBitReverso();
         	fft();
     }
     
    	imprimeVetor();

     return 0;

}

