#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <sys/time.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define NUM_ITERACOES 1
#define ISIGN 1


//dados globais usados pelas threads
typedef struct strDadosThread
{
    
    unsigned long tamArray;
    int qtdThreads;
    unsigned long m;
    unsigned long mmax;
    unsigned long istep;
    float wr;
    float wi; 
    unsigned long tamBloco;
}DadosThread;

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


//dados globais
float *data, *cudaData;
DadosThread *dadosThreads, *cudaDadosThreads;
int qtdThreads, qtdThreadsBlocos, pesoThreads;
unsigned long qtdElementos, tamArray;


void imprimeVetor(){
    int posicao;
    for(posicao=1;posicao<tamArray;posicao++){
        printf("%2.2f ",data[posicao]);
    }
    printf("\n\n");
}

void ordenaBitReverso(){
    
    unsigned long i,j,m;
    float tempr;
    j=1;
    
    for (i=1;i<tamArray;i+=2) {
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
}

void inicializaArray(){
    
    unsigned long i;
    
    for (i=1; i<tamArray; i+=8) {
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

void calculoButterflyBlocoLocal(float *data, DadosThread *dthread){

	unsigned long m = dthread->m;
	unsigned long mmax = dthread->mmax; 
	unsigned long istep = dthread->istep;
	unsigned long tamBloco = dthread->tamBloco; 
	float wr = dthread->wr;
	float wi = dthread->wi;
	float tempr,tempi;
	unsigned long j,i,bloco;
	bloco=1;

	for (i=m;bloco<=tamBloco;i+=istep) {
		j=i+mmax;                                
		//printf("          >>> FOR 2 -> wi:%f wr:%f istep:%lu mmax:%lu i:%lu j:%lu\n",wi,wr,istep, mmax, i, j);
		tempr=wr*data[j]-wi*data[j+1];
		tempi=wr*data[j+1]+wi*data[j];
		data[j]=data[i]-tempr;
		data[j+1]=data[i+1]-tempi;
		data[i] += tempr;
		data[i+1] += tempi; 
		bloco++;
	}
}


__global__ void calculoButterflyBlocoCuda(float *data, DadosThread *dthread){

	int threadID = blockIdx.x * blockDim.x + threadIdx.x;

	unsigned long m = (((dthread->tamArray - 1)/dthread->qtdThreads)*threadID)+dthread->m;
	unsigned long mmax = dthread->mmax; 
	unsigned long istep = dthread->istep;
	unsigned long tamBloco = dthread->tamBloco; 
	float wr = dthread->wr;
	float wi = dthread->wi;
	float tempr,tempi;
	unsigned long j,i,bloco;
	bloco=1;

	for (i=m;bloco<=tamBloco;i+=istep) {
		j=i+mmax;                                
		//printf("          >>> FOR 2 -> wi:%f wr:%f istep:%lu mmax:%lu i:%lu j:%lu\n",wi,wr,istep, mmax, i, j);
		tempr=wr*data[j]-wi*data[j+1];
		tempi=wr*data[j+1]+wi*data[j];
		data[j]=data[i]-tempr;
		data[j+1]=data[i+1]-tempi;
		data[i] += tempr;
		data[i+1] += tempi; 
		bloco++;
	}
}

int fftCuda(){


	unsigned long mmax,m,istep;
	unsigned long tamLoop,tamBloco;
	float wtemp,wr,wpr,wpi,wi,theta;
	mmax=2;
	tamLoop = qtdElementos/mmax;
	tamBloco = tamLoop/qtdThreads;
	int processaGPU = 0; 	
	int retornouDadosGPU = 0; 
	unsigned long totalPeso = (pow(qtdThreads,pesoThreads));
	

	if(qtdThreads > 1 && tamLoop >= totalPeso){

		processaGPU = 1;

		//aloca espaco para os vetores na GPU
		if (cudaMalloc (&cudaData, sizeof(float) * tamArray) != cudaSuccess) 
		{ printf("Erro cudaMalloc do array 'data'\n"); return -1; }

		//Copia os dados da memoria principal para a memoria do dispositivo
		if (cudaMemcpy(cudaData, data, sizeof(float) * tamArray, cudaMemcpyHostToDevice) != cudaSuccess) 
		{ printf("Erro cudaMemcpy do array 'data'\n"); return -1; }

		//aloca espaco para os dados usados no algoritmo na GPU
		if (cudaMalloc (&cudaDadosThreads, sizeof(DadosThread)) != cudaSuccess) 
		{ printf("Erro cudaMalloc do struct 'DadosThread'\n"); return -1; }	

	}
	
   
        while (qtdElementos >=  mmax) {
        //printf(">>> Inicio do while -> mmax:%lu\n",mmax);
                istep=mmax << 1;
                theta=ISIGN*(6.28318530717959/mmax);
                wtemp=sin(0.5*theta);
                wpr = -2.0*wtemp*wtemp;
                wpi=sin(theta);
                wr=1.0;
                wi=0.0;
                for (m=1;m<mmax;m+=2) {
                //printf("     >>> FOR 1 -> m:%lu mmax:%lu\n",m,mmax); 

			tamLoop = qtdElementos/mmax;
			tamBloco = tamLoop/qtdThreads;

			if(tamLoop < totalPeso && qtdThreads > 1){
				processaGPU = 0;
				if(!retornouDadosGPU){
					if (cudaMemcpy(data, cudaData, sizeof(float) * tamArray, cudaMemcpyDeviceToHost) != cudaSuccess){ 
						printf("Erro cudaMemcpy array 'data'\n"); return -1; 
					}
					retornouDadosGPU = 1;
				}			
			}
                                     
			//preeenche os dados do processamento
			dadosThreads->tamArray = tamArray;
    			dadosThreads->qtdThreads = qtdThreads;
			dadosThreads->m = m;
			dadosThreads->mmax = mmax;
			dadosThreads->istep = istep;
			dadosThreads->wr = wr;
			dadosThreads->wi = wi; 
			dadosThreads->tamBloco = tamBloco;

			if(processaGPU){
				
				//printf("vai processar na GPU tamLoop:%lu tamBloco:%lu\n",tamLoop,tamBloco);

				dadosThreads->tamBloco = tamBloco;

				//Copia os dados do execucao para a memoria do dispositivo
				if (cudaMemcpy(cudaDadosThreads, dadosThreads, sizeof(DadosThread), cudaMemcpyHostToDevice) != cudaSuccess) 
					{ printf("Erro cudaMemcpy do array 'data'\n"); return -1; }

				//executa o kernel
				calculoButterflyBlocoCuda<<<qtdThreads/qtdThreadsBlocos,qtdThreadsBlocos>>>(cudaData,cudaDadosThreads);
			}else{
				//printf("vai processar localmente tamLoop:%lu\n",tamLoop);
				//senao calcula localmente
				dadosThreads->tamBloco = tamLoop;
				calculoButterflyBlocoLocal(data,dadosThreads);
			}

			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
                }

		/*if(processaGPU){
			//obtem o istep da GPU
			if (cudaMemcpy(dadosThreads, cudaDadosThreads, sizeof(unsigned long), cudaMemcpyDeviceToHost) != cudaSuccess) { 
				printf("Erro cudaMemcpy do 'cudaDadosThreads'\n"); return -1; 
			}
		}*/

		//e atualiza os valores internos para a proxima rodada
                mmax=istep;
        //printf("<<< Final do while -> istep:%lu mmax:%lu\n\n",istep,mmax);
        }

	//libera a memoria na GPU
	if (cudaFree(cudaData) != cudaSuccess) { printf("Erro cudaFree 'cudaData'\n"); return -1; }
	if (cudaFree(cudaDadosThreads) != cudaSuccess) { printf("Erro cudaFree 'cudaDadosThreads'\n"); return -1; }
	
	//retorna sucesso
	return 0;

}

int main(int argc, char *argv[]) {

	struct timeval inicio, fim;
	tsc_counter tsc1, tsc2;
	long long unsigned int clock;
	float tempo;

	if(argc < 5) {
	        printf("ERRO: informe s quantidade de elementos do FFT, a quantidade de threads e a quantidade de threads por blocos e o peso das threads:: fft-cuda <qtdElementos> <qtdThreads> <qtdThreadsPorBloco> <pesoThreads>\n");
	        return -1;
	}
    
	//inicializando valores do algoritmo
	qtdElementos = strtoul(argv[1],NULL,10);
	tamArray = (qtdElementos * 2) + 1;
	qtdThreads = atoi(argv[2]);
	qtdThreadsBlocos = atoi(argv[3]);
	pesoThreads = atoi(argv[4]);
    
	//printf("sera processada uma fft com Elementos:%lu, Threads:%d, PesoThread:%d\n",qtdElementos,qtdThreads,quantidadeBlocos);

	//alocando memoria para o vetor de dados
	data = (float*)malloc(sizeof(float) * tamArray);
	if(data==NULL) { printf("Erro malloc vetor 'data'\n"); return(-1); }

	//alocando memoria para o struct dos dados da thread
	dadosThreads = (DadosThread*)malloc(sizeof(DadosThread));
	if(data==NULL) { printf("Erro malloc no struct 'DadosThread'\n"); return(-1); }


	//warmup
	//inicializaArray();
	//ordenaBitReverso();
	//fftCuda();

	//repete as execucoes para calcular a media
	unsigned long count;
    
	//mede o tempo de execução da funcao (media de NUM_ITERACOES repeticoes) 
	gettimeofday(&inicio, NULL);
	RDTSC(tsc1);
	for(count=0;count<NUM_ITERACOES;count++){
        
	        inicializaArray();
	        //imprimeVetor();
        
	        ordenaBitReverso();
	        //imprimeVetor();
        
	        fftCuda();
	}

	gettimeofday(&fim, NULL);
	RDTSC(tsc2);
	tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
	clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
	printf("%lu\t%d\t%d\t%d\t%.1lf\t%.2e\t%.2e\n",qtdElementos,qtdThreads,qtdThreadsBlocos,pesoThreads,tempo/NUM_ITERACOES,(float)clock/NUM_ITERACOES,clock/tempo);

	//imprimeVetor();

	//liberando a memoria
	free(data);
	free(dadosThreads);

	return 0;
}

