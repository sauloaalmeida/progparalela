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
				printf("         j=i+mmax; -> j=%lu, i=%lu, mmax=%lu ",j,i,mmax);
				j=i+mmax;
			        printf("         j depois de atualizado -> j=%lu\n",j);
				
				printf("         tempr = wr*data[j]-wi*data[j+1]; -> j=%lu, data[j+1]=%f, wi=%f, wr=%f, data[j]=%f ",j,data[j+1], wi, wr, data[j]);
				tempr = wr*data[j]-wi*data[j+1];
				printf("         tempr=%f\n",tempr);

					
                                printf("         tempi = wr*data[j+1]+wi*data[j]; -> data[j]=%f, wi=%f, data[j+1]=%f, wr=%f ",data[j], wi, data[j+1],wr);
				tempi = wr*data[j+1]+wi*data[j];
				printf("         tempi=%f\n",tempi);

				
				printf("         data[j] = data[i]-tempr; -> tempr=%f data[i]=%f, i=%lu, j=%lu ",tempr,data[i],i,j);
				data[j] = data[i]-tempr;
   			        printf("         data[j]=%f j=%lu\n",data[j],j);


				printf("         data[j+1] = data[i+1]-tempi; -> tempi=%f, data[i+1]=%f, i=%lu, j=%lu ",tempi,data[i+1],i,j);
				data[j+1] = data[i+1]-tempi;
				printf("         data[j+1]=%f j=%lu\n",data[j+1],j);
				

				printf("         data[i] = data[i] +tempr; -> tempr=%f, data[i]=%f, i=%lu ",tempr,data[i],i);
				data[i] = data[i]+tempr;
				printf("         data[i]=%f i=%lu\n",data[i],i);

				
				printf("         data[i+1] = data[i+1] + tempi; -> tempi=%f, data[i+1]=%f, i=%lu ",tempi,data[i+1],i);
				data[i+1] = data[i+1] + tempi;
				printf("         data[i+1]=%f i=%lu\n",data[i+1],i);

                        printf("         >> final do for interno 2\n");
			}
			printf("      wr = (wtemp=wr)*wpr-(wi*wpi)+wr; - wr=%f,wpi=%f,wi=%f,wpr=%f,wtemp=%f => ",wr,wpi,wi,wpr,wtemp);
			wr = (wtemp=wr)*wpr-(wi*wpi)+wr;
			printf("      wr=%f\n",wr);
 
                        printf("      wi = (wi*wpr)+(wtemp*wpi)+wi; - wi=%f,wpi=%f,wtemp=%f,wpr=%f => ",wi,wpi,wtemp,wpr);
			wi = (wi*wpr)+(wtemp*wpi)+wi;
                        printf("      wi=%f\n",wi);

                printf("      << final do for interno 1\n\n");                        
		}                
                printf("   mmax=istep;");
		mmax=istep;//caminha o proximo passo no laco mais externo
                printf("  mmax=%lu\n",mmax);
        printf("   << final do loop externo do calculo do fourier\n");


	}

        printf("<< final da rotina de calculo do fourier\n");

}


int main(void) {

		float data[32] = {0.,0.,1.,0.,2.,0.,3.,0.,0.,0.,1.,0.,2.,0.,3.,0.,0.,0.,1.,0.,2.,0.,3.,0.,0.,0.,1.,0.,2.,0.,3.,0.} ;
                fftLegivel(data-1,16,1);
                imprimeVetor(data,32);

		//float data[16] = {0.,0.,1.,0.,2.,0.,3.,0.,0.,0.,1.,0.,2.,0.,3.,0.} ;
		//fftLegivel(data-1,8,1);
		//imprimeVetor(data,16);

		//float data[8] = {0.,0.,1.,0.,2.,0.,3.,0.};
		//fftLegivel(data-1,4,1); 
		//imprimeVetor(data,8);

		return 0;

}


