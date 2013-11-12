#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#define NITER 100000
#define SWAP(a,b) tempr=a;a=b;b=tempr


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
  
void fft(float data[], unsigned long qtdElementos, int isign){

	unsigned long tamArray,mmax,j,i,m,istep;
	double wtemp,wr,wpr,wpi,wi,theta;

	float tempr,tempi;

	tamArray=qtdElementos<<1;
		
	j=1;

	for (i=1;i<tamArray;i+=2) {                
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
                istep=mmax << 1;
		theta=isign * (6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=tamArray;i+=istep) {				
				j=i+mmax;
				tempr = wr*data[j]-wi*data[j+1];
				tempi = wr*data[j+1]+wi*data[j];
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

     struct timeval inicio, fim;
     tsc_counter tsc1, tsc2;
     long long unsigned int clock;
     double tempo;
     int j=1;

     //wurmup
     executaFFT16384();

     //mede o tempo de execução da funcao f0 (media de NITER repeticoes) 
     gettimeofday(&inicio, NULL); 
     RDTSC(tsc1);
     for (j=0; j<NITER; j++) {
          executaFFT16384();
     }

     RDTSC(tsc2);
     gettimeofday(&fim, NULL);
     tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
     //printf("tempo:%lf",tempo);
     clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
     printf("Tempo FFT : %.1lf(ms) Clocks: %.2e \n", tempo/NITER, (double)clock/NITER);
     printf("Clock/tempo: %.2e\n\n", clock/tempo);


}
