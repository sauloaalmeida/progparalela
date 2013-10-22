/*
 ============================================================================
 Name        : fft2.c
 Author      : saluo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr

void imprimeInversa(float dadosImp[], int tamVet){
	int posicao;
	for(posicao=0;posicao<tamVet;posicao++){
		   printf("%f ",dadosImp[posicao]/(tamVet/2));
	}
    printf("\n");
}

void imprimeModulo(float dadosImp[], int tamVet){
	int posicao;
	for(posicao=0;posicao<tamVet;posicao=posicao+2){
	       printf("%f ",sqrt(pow(dadosImp[posicao],2)+pow(dadosImp[posicao+1],2)));
	}
    printf("\n");
}

void imprimeVetor(float dadosImp[], int tamVet){
	int posicao;
	for(posicao=0;posicao<tamVet;posicao++){
	       printf("%f ",dadosImp[posicao]);
	}
    printf("\n\n");
}

void imprimeVetorDouble(double dadosImp[], int tamVet){
	int posicao;
	for(posicao=0;posicao<tamVet;posicao++){
	       printf("%f ",dadosImp[posicao]);
	}
    printf("\n\n");
}

void fftLivro2(float data[], unsigned long nn, int isign)
{
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
		m=n >> 1;
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

/******************************************************************************/
void fftLivro(double data[], unsigned long nn, int isign)
/*******************************************************************************
Replaces data[1..2*nn] by its discrete Fourier transform, if isign is input as
1; or replaces data[1..2*nn] by nn times its inverse discrete Fourier transform,
if isign is input as -1.  data is a complex array of length nn or, equivalently,
a real array of length 2*nn.  nn MUST be an integer power of 2 (this is not
checked for!).
*******************************************************************************/
{
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	double tempr,tempi;

	n=nn << 1;
	j=1;
	//imprimeVetor(data,n);
	for (i=1;i<n;i+=2) { /* This is the bit-reversal section of the routine. */
		if (j > i) {
			SWAP(data[j],data[i]); /* Exchange the two complex numbers. */
			SWAP(data[j+1],data[i+1]);
		}
		m=nn;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	//imprimeVetor(data,n);
	mmax=2;
	while (n > mmax) { /* Outer loop executed log2 nn times. */
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax); /* Initialize the trigonometric recurrence. */
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) { /* Here are the two nested inner loops. */
			for (i=m;i<=n;i+=istep) {
				j=i+mmax; /* This is the Danielson-Lanczos formula. */
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr; /* Trigonometric recurrence. */
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}




void fftArtigo(float *data, int *qtdElementos, int *isign){



     int tamVetor, mmax, m, j, i;
     double wtemp, wr, wpr, wpi, wi, theta, wpin;
     double tempr, tempi, datar, datai,data1r,data1i;

     theta = 3.141592653589795 * .5;
     tamVetor = *qtdElementos * 2;
     j = 0;

     for(i = 0; i < tamVetor; i += 2){
          if (j > i) {
               SWAP(data[j], data[i]);
               SWAP(data[j + 1], data[i + 1]);
          }

          m = *qtdElementos;
          while (m >= 2 && j >= m) {
               j -= m;
               m >>= 1;
          }

          j += m;
     }

     if (*isign < 0){
          theta = -theta;
     }

     wpin = 0;

     for(mmax = 2; tamVetor > mmax; mmax *= 2){
          wpi = wpin;
          wpin = sin(theta);
          wpr = 1 - wpin * wpin - wpin * wpin;
          theta *= .5;
          wr = 1;
          wi = 0;
          for(m = 0; m < mmax; m += 2){
               j = m + mmax;
               tempr = (double) wr *(data1r = data[j]);
               tempi = (double) wi *(data1i = data[j + 1]);
               for(i = m; i < tamVetor - mmax * 2; i += mmax * 2){
                    tempr -= tempi;
                    tempi = (double) wr *data1i + (double) wi *data1r;
                    data1r = data[j + mmax * 2];
                    data1i = data[j + mmax * 2 + 1];
                    data[i] = (datar = data[i]) + tempr;
                    data[i + 1] = (datai = data[i + 1]) + tempi;
                    data[j] = datar - tempr;
                    data[j + 1] = datai - tempi;
                    tempr = (double) wr *data1r;
                    tempi = (double) wi *data1i;
                    j += mmax * 2;
               }

               tempr -= tempi;
               tempi = (double) wr *data1i + (double) wi *data1r;
               data[i] = (datar = data[i]) + tempr;
               data[i + 1] = (datai = data[i + 1]) + tempi;
               data[j] = datar - tempr;
               data[j + 1] = datai - tempi;
               wr = (wtemp = wr) * wpr - wi * wpi;
               wi = wtemp * wpi + wi * wpr;
          }
     }
}

void fft(float data[], unsigned long nn, int isign){

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

void fftLegivel(float data[], unsigned long qtdElementos, int isign){

	unsigned long tamArray,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;

	float tempr,tempi;

	tamArray=qtdElementos * 2;
        printf("tamArray=%lu\n",tamArray);
        printf("Array de entrada: ");
        imprimeVetor(data,tamArray);
	j=1;
        printf("j=1\n");

	printf(">> inicio rotina de bit reverso\n");
	for (i=1;i<tamArray;i+=2) {
                printf("   >> loop bit reverso: j=%lu, i=%lu\n",j,i);
		if (j > i) {
                        printf("   loop bitreverso: entrou no IF: troca i=%lu(real:%f;imag:%f), com j=%lu(real:%f;imag:%f)\n",i,data[i],data[i+1],j,data[j],data[j+1]);
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=qtdElementos;
                printf("\n   m=%lu\n",m);
		while (m >= 2 && j > m) {
			printf("      bitreverso loop while: m=%lu j=%lu \n",m,j);			
			j = j-m;
			m = m/2;
			printf("         bitreverso loop while apos atualizacao: m=%lu j=%lu \n",m,j);
		}
                printf("   bitreverso depois do  while: m=%lu j=%lu \n",m,j);
		j = j+m;
		printf("   bitreverso depois do while, com j atualizado: j = j+m; j=%lu \n",j);
                
	}
        printf("<< fim rotina de bit reverso\n\n");


        printf(">> inicio rotina de calculo do fourier\n");

	mmax=2; //divide em 2 blocos (pares e impares)

	while (tamArray > mmax) {
        printf("   >> inicio do loop externo do calculo do fourier\n");
                printf("   mmax=2\n");
                printf("   istep=mmax * 2;");
		istep=mmax * 2; //tamanho do bloco
                printf("   istep=%lu\n",istep);

		printf("   theta=isign * (6.28318530717959/mmax);");
		theta=isign * (6.28318530717959/mmax); // theta = signal * (pi/2) 1 iteracao = pi 3,141543
                printf("   theta=%f\n",theta);

		wtemp=sin(0.5*theta); //wtemp = seno(3,141543/2)
		wpr = -2.0*wtemp*wtemp; //wpr = -2.wtemp^2
		wpi=sin(theta); //wpi = sen(3,141543)
		wr=1.0; //wr = 1
		wi=0.0; //wi = 0
		for (m=1;m<mmax;m+=2) {
		printf("      >> inicio do for interno 1 - for (m=1;m<mmax;m+=2) m=%lu, mmax=%lu\n",m,mmax);
			for (i=m;i<=tamArray;i+=istep) {
                        printf("         >> inicio do for interno 2 - for (i=m;i<=tamArray;i+=istep) i=%lu, m=%lu, istep=%lu\n",i,m,istep);
				j=i+mmax;
				tempr = wr*data[j]-wi*data[j+1];
				tempi = wr*data[j+1]+wi*data[j];
				data[j] = data[i]-tempr;
				data[j+1] = data[i+1]-tempi;
				data[i] = data[i] +tempr;
				data[i+1] = data[i+1] + tempi;
                        printf("         >> final do for interno 2\n");
			}
			printf("      wr = (wtemp=wr)*wpr-(wi*wpi)+wr; - wr=%f,wpi=%f,wi=%f,wpr=%f,wtemp=%f => ",wr,wpi,wi,wpr,wtemp);
			wr = (wtemp=wr)*wpr-(wi*wpi)+wr;
			printf("      wr=%f\n",wr);
 
                        printf("      wi = (wi*wpr)+(wtemp*wpi)+wi; - wi=%f,wpi=%f,wtemp=%f,wpr=%f => ",wi,wpi,wtemp,wpr);
			wi = (wi*wpr)+(wtemp*wpi)+wi;
                        printf("      wi=%f\n",wi);
                        
		}
                printf("      << final do for interno 1\n");
                printf("   mmax=istep;");
		mmax=istep;//caminha o proximo passo no laco mais externo
                printf("  mmax=%lu\n",mmax);
        printf("   << final do loop externo do calculo do fourier\n");


	}

        printf("<< final da rotina de calculo do fourier\n");

}


int main(void) {

		//float data[16] = {0.,0.,1.,0.,2.,0.,3.,0.,0.,0.,1.,0.,2.,0.,3.,0.} ;
		//fftLegivel(data-1,8,1);
		//imprimeVetor(data,16);

		float data[8] = {0.,0.,1.,0.,2.,0.,3.,0.};
		fftLegivel(data-1,4,1); 
		imprimeVetor(data,8);

		return 0;

}


