#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define NUM_ITERACOES 10
#define ISIGN 1


//dados globais usados pelas threads
typedef struct strDadosThread
{
    unsigned long m;
    unsigned long mmax;
    unsigned long istep;
    unsigned long tamArray;    
    double wr;
    double wi; 
    float dados;
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


int qtdProcessos;

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





void fftMpi(){

     int rank;
     unsigned long mmax,j,m,istep,i;
     double wtemp,wr,wpr,wpi,wi,theta;
     unsigned long totalPeso = (pow(qtdProcessos,pesoThreads));

    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &qtdProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    

    int root = 0;

    if(rank==root){
        mmax=2;
        while (tamArray > mmax) {
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

                    unsigned long blocoAtual;            
                    if (qtdProcessos > 1 && qtdElementos/mmax >= totalPeso) {
                        //printf("     processa em thread\n");
                        unsigned long tamBloco = qtdElementos/mmax/qtdProcessos;
                
                        //loop das threads pela quantidade de cores
                        for (blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++) {
                            //printf("     qtdBlocos:%lu\n",blocoAtual);
                    
                            //preeenche os dados para enviar para a thread
                            //dadosThreads[blocoAtual].m = (((tamArray - 1)/qtdProcessos)*blocoAtual)+m;
                            dadosThreads.m = m;                            
                            dadosThreads.mmax = mmax;
                            dadosThreads.istep = istep;
                            dadosThreads.wr = wr;
                            dadosThreads.wi = wi; 
                            dadosThreads.tamBloco = tamBloco;
                                			
                            //manda para o MPI fazer o calculo
                            
                            
                            //recebe do MPI o resultado
                            //atualiza o vetor principal
                            
                        }

                		//--barreira que espera todas as threads terminarem

                
				    } else {
				        //printf("     processa na main\n");
				        calculoButterflyBloco(m,mmax,istep,wr,wi,qtdElementos/mmax);
				    }

                  wr=(wtemp=wr)*wpr-wi*wpi+wr;
                  wi=wi*wpr+wtemp*wpi+wi;
                }
                mmax=istep;
        //printf("<<< Final do while -> istep:%lu mmax:%lu\n\n",istep,mmax);
        }
	}

    MPI_Finalize();
        
}












int main(int argc, char *argv[]) {

     struct timeval inicio, fim;
     tsc_counter tsc1, tsc2;
     long long unsigned int clock;
     double tempo;
        unsigned long count;
    
    if(argc < 3) {
        printf("ERRO: informe s quantidade de elementos do FFT, o peso das threads:: <./fft-mpi> <qtdElementos> <pesoThreads>\n");
        return -1;
    }
    
    //inicializando valores do algoritmo
    qtdElementos = strtoul(argv[1],NULL,10);
    tamArray = qtdElementos*2 + 1;
    pesoThreads = atoi(argv[2]);

    //alocando memoria para o vetor de dados
    data = malloc(sizeof(float) * tamArray);
    
    //alocando a memoria para os dados das threads
    dadosThreads = malloc(sizeof(DadosThread));

    
     //wurmup
     inicializaArray();
     ordenaBitReverso();
     fftMpi();

     //mede o tempo de execução da funcao f0 (media de NITER repeticoes) 
     gettimeofday(&inicio, NULL);
     RDTSC(tsc1);
        for(count=0;count<NUM_ITERACOES;count++){
             inicializaArray();
             ordenaBitReverso();
             fftMpi();
        }

     RDTSC(tsc2);
     gettimeofday(&fim, NULL);
     tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
     //printf("tempo:%lf",tempo);
     clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
     //printf("Tempo FFT : %.1lf(ms) Clocks: %.2e \n", tempo/NUM_ITERACOES, (double)clock/NUM_ITERACOES);
     //printf("Clock/tempo: %.2e\n", clock/tempo);
     //printf("QtdElementos:%lu Threads: %d Peso:%d\n",qtdElementos,qtdProcessos,pesoThreads);
      printf("%lu\t%d\t%d\t%.1lf\t%.2e\t%.2e\n",qtdElementos,qtdProcessos,pesoThreads,tempo/NUM_ITERACOES,(double)clock/NUM_ITERACOES,clock/tempo);
        //imprimeVetor();
    
    //liberando a memoria
    free(dadosThreads);
    free(data);

    return 0;

}
