#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define SWAP(a,b) tmp=a;a=b;b=tmp

void fft(float data[], unsigned long qtdElementos, int isign){

	unsigned long tamArray,mmax,j,i;
	double wtemp,wr,wpr,wpi,wi;
    float tmp;

	tamArray=qtdElementos<<1;
		
	j=1;

	for (i=1;i<tamArray;i+=2) {
                unsigned long m;
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=qtdElementos;
		while (m >= 2 && j > m) {
			j = j-m;
			m = m/2;
		}
		j = j+m;               
	}


	mmax=2;
	while (tamArray > mmax) {
        unsigned long m,i;
		unsigned long istep=mmax << 1;
		double theta=isign * (6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=tamArray;i+=istep) {				
				j=i+mmax;
				float tempr = wr*data[j]-wi*data[j+1];
				float tempi = wr*data[j+1]+wi*data[j];
				data[j] = data[i]-tempr;
				data[j+1] = data[i+1]-tempi;
				data[i] = data[i]+tempr;
				data[i+1] = data[i+1] + tempi;
			}
			wr = (wtemp=wr)*wpr-(wi*wpi)+wr;
			wi = (wi*wpr)+(wtemp*wpi)+wi;
		}                
		mmax=istep;
	}
}

int main(void) {
		float data[8] = {0.,0.,1.,0.,2.,0.,3.,0.};
		fftLegivel(data-1,4,1); 
		return 0;
}


