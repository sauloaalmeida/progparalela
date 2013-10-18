#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
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

void fftArtigo(float *data, int *qtdElementos, int *isign){



     int tamVetor, mmax, m, j, i;
     double wtemp, wr, wpr, wpi, wi, theta, wpin;
     double tempr, tempi, datar, datai,data1r,data1i;

     theta = 3.141592653589795 * .5;
     tamVetor = *qtdElementos * 2;
     j = 0;

     //imprimeVetor(data,tamVetor);

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

//     imprimeVetor(data,tamVetor);
}


void fftLivro(float data[], unsigned long qtdElementos, int isign){

	unsigned long tamVetor,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	float tempr,tempi;

	tamVetor=qtdElementos << 1;

	//imprimeVetor(data,tamVetor);

	j=1;
	for (i=1;i<tamVetor;i+=2) {

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

	mmax=2;
	while (tamVetor > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=tamVetor;i+=istep) {
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

	//imprimeVetor(data,tamVetor);

}

int main (){

    int *signP,*tamP;
    float *arr;

     //float dados[16]={0,0,1,0,2,0,3,0,4,0,5,0,6,0,7,0};
     //fftLivro(dados,8,1);
     float dados[8]={0,0,1,0,2,0,3,0};
     fftLivro(dados,4,-1);


     /*float dados3[16]={1,0,2,-1,3,-2,3,0,1,0,-2,0,1,1,2,0};
     fftLivro(dados3,8,-1);*/

     //float dados2[8]={0,0,1,0,2,0,3,0};
     //float dados2[8]={6.000000,0.000000,-2.000000,-2.000000,-2.000000,0.000000,-2.000000,2.000000};

     //arr = malloc(sizeof(float) * 16);
     arr = malloc(sizeof(float) * 8);
     if(arr==NULL) {printf("ERRO: alocação do vetor\n"); return -1;}

     arr[0]=6;
     arr[1]=0;
     arr[2]=-2;
     arr[3]=-2;
     arr[4]=-2;
     arr[5]=0;
     arr[6]=-2;
     arr[7]=2;

     /*arr[0]=0;
     arr[1]=0;
     arr[2]=1;
     arr[3]=0;
     arr[4]=2;
     arr[5]=0;
     arr[6]=3;
     arr[7]=0;*/

     /*arr[8]=4;
     arr[9]=0;
     arr[10]=5;
     arr[11]=0;
     arr[12]=6;
     arr[13]=0;
     arr[14]=7;
     arr[15]=0;
*/
     //int tam=8;
     int tam=4;
     tamP=&tam;

     //int sign=1;
     int sign=-1;
     signP=&sign;

     fftArtigo(arr,tamP,signP);
     imprimeInversa(arr,8);

     return -1;
}
