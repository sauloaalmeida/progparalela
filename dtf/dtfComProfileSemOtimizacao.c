#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define NITER 1000000
#define SWAP(a,b) tempr=a;a=b;b=tempr


void four1(float *data, int *nn, int *isign){ 

     /* altered for consistency with original FORTRAN.
        Press, Flannery, Teukolsky, Vettering "Numerical
        Recipes in C" tuned up ; Code works only when *nn is
        a power of 2 */

     int n, mmax, m, j, i;
     double wtemp, wr, wpr, wpi, wi, theta, wpin;
     double tempr, tempi, datar, datai,data1r,data1i;
     n = *nn * 2;
     j = 0;


     for(i = 0; i < n; i += 2){ 
          if (j > i) {
               /* could use j>i+1 to help
                * compiler analysis */
               SWAP(data[j], data[i]);
               SWAP(data[j + 1], data[i + 1]);
          }

          m = *nn;
          while (m >= 2 && j >= m) {
               j -= m;
               m >>= 1;
          }

          j += m;
     }

     theta = 3.141592653589795 * .5;

     if (*isign < 0)
          theta = -theta;

     wpin = 0;          /* sin(+-PI) */

     for(mmax = 2; n > mmax; mmax *= 2){ 
          wpi = wpin;
          wpin = sin(theta);
          wpr = 1 - wpin * wpin - wpin * wpin;
          /* cos(theta*2) */
          theta *= .5;
          wr = 1;
          wi = 0;
          for(m = 0; m < mmax; m += 2){ 
               j = m + mmax;
               tempr = (double) wr *(data1r = data[j]);
               tempi = (double) wi *(data1i = data[j + 1]);
               for(i = m; i < n - mmax * 2; i += mmax * 2){ 
                    /* mixed precision not significantly more
                    * accurate here; if removing double casts,
                    * tempr and tempi should be double */
                    tempr -= tempi;
                    tempi = (double) wr *data1i + (double) wi *data1r;
                    /* don’t expect compiler to analyze j > i+1 */
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



int carregaVetor(float *vet, FILE *fp, int tamanhoVetor) {
   int i;
   //printf("tamanho do vetor: %d", tamanhoVetor);
   for (i=0; i<tamanhoVetor; i++){
         if(fscanf(fp, "%f", &vet[i])==0){ 
               return 0;
         }
         //printf("vetor[%d]=%f\n", i,vet[i]);
    }     
   return 1;
}

int main(int argc, char *argv[]){

     int j=1;
     int positivoValue=1;
     int *positivo;
     int tamVetValue=0;
     int *tamVet;
     float *array_f;
     FILE *fA;
     
     tamVet = &tamVetValue;
     positivo = &positivoValue; 

     if(argc < 2) {
        printf("ERRO: lista de argumentos incorreta: \n<prog> <arq-vetor> \n");
        return -1;
     } 

     fA = fopen(argv[1], "r");
     if(fA==NULL) {printf("ERRO: leitura do arquivo com o vetor\n"); return -1;}

     if(fscanf(fA, "%d", tamVet)==0) {
        printf("erro ao ler o tamanho do vetor");
        return -1; //numero de colunas do vetor
     }
   
     array_f = malloc(sizeof(float) * (*tamVet));
     if(array_f==NULL) {printf("ERRO: alocação do vetor\n"); return -1;}
   
     //carrega os valores iniciais das matrizes
     if(!carregaVetor(array_f, fA, *tamVet)) return -1;
 
     //fecha o arquivos de entrada
     fclose(fA);
     //printf("Arquivo Fechado\n");
    
     //executa NITER vezes
     for (j=0; j<NITER; j++) {
          four1(array_f,tamVet,positivo);
     }

     return 1;
}


