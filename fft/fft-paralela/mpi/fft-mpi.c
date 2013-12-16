#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define NUM_ITERACOES 1
#define ISIGN 1


//dados globais usados pelas threads
typedef struct strDadosExecucaoProcesso
{
    unsigned long m;
    unsigned long mmax;
    unsigned long istep;
    unsigned long tamBloco;            
    double wr;
    double wi; 
}DadosExecucaoProcesso;


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


int qtdProcessos,totalProcessos,pesoThreads;
unsigned long qtdElementos, tamArray;

void imprimeVetor(float data[]){
    int posicao;
    for(posicao=1;posicao<tamArray;posicao++){
        printf("%2.2f ",data[posicao]);
    }
    printf("\n\n");
}

void ordenaBitReverso(float data[]){
    
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

void inicializaArray(float data[]){
    
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


void calculoButterflyBloco(float dataBloco[], unsigned long m,unsigned long mmax, unsigned long istep, double wr, double wi, unsigned long tamBloco){
        
    float tempr,tempi;
    unsigned long j,i,bloco;
    bloco=1;
    
    for (i=m;bloco<=tamBloco;i+=istep) {
        j=i+mmax;                                
        printf("          >>> FOR 2 -> wi:%f wr:%f istep:%lu mmax:%lu i:%lu j:%lu\n",wi,wr,istep, mmax, i, j);
        tempr=wr*dataBloco[j]-wi*dataBloco[j+1];
        tempi=wr*dataBloco[j+1]+wi*dataBloco[j];
        dataBloco[j]=dataBloco[i]-tempr;
        dataBloco[j+1]=dataBloco[i+1]-tempi;
        dataBloco[i] += tempr;
        dataBloco[i+1] += tempi; 
        bloco++;
    }
}


void fftMpi(int argc, char *argv[]){

    float *dataLocal;
    int rank;
    unsigned long mmax,j,m,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    unsigned long totalPeso = (pow(qtdProcessos,pesoThreads));
     
    DadosExecucaoProcesso     dadosExecucaoProcesso;
	MPI_Datatype dadosExecucaoProcessoType, oldtypes[2];
	int          blockcounts[2];
	MPI_Aint    offsets[2], extent;

    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    
    
    //inicializacao dos campos unsigned longs
    offsets[0] = 0;
    oldtypes[0] = MPI_UNSIGNED_LONG;
    blockcounts[0] = 4;
    
    //inicializacao dos campos double
    //precisa primero descobrir o offset pelo tamanho do MPI_UNSIGNED_LOG
	MPI_Type_extent(MPI_UNSIGNED_LONG, &extent);
	offsets[1] = 4 * extent;
	oldtypes[1] = MPI_DOUBLE;
	blockcounts[1] = 2;    
	
	//Agora define o tipo estruturado e commita
	MPI_Type_struct(2, blockcounts, offsets, oldtypes, &dadosExecucaoProcessoType);
	MPI_Type_commit(&dadosExecucaoProcessoType);	


    int root = totalProcessos-1;
    qtdProcessos = root;

    if(rank==root){
    
      	float data[tamArray];
      	 
        //alocando memoria para o vetor de dados
//        data = malloc(sizeof(float) * tamArray);    

        //inicializa o array        
        inicializaArray(data);
        //e ordena o bit reverso
        ordenaBitReverso(data);
    
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
                printf("     >>> FOR 1 -> m:%lu mmax:%lu\n",m,mmax); 

                    unsigned long blocoAtual;            
                    if (qtdProcessos > 1 && qtdElementos/mmax >= totalPeso) {
                        //printf("     processa em thread\n");
                        unsigned long tamBloco = qtdElementos/mmax/qtdProcessos;
                        
                            //preenche os dados para enviar para os processos
                            //dadosThreads[blocoAtual].m = (((tamArray - 1)/qtdProcessos)*blocoAtual)+m;
                            dadosExecucaoProcesso.m = m;
                            dadosExecucaoProcesso.mmax = mmax;
                            dadosExecucaoProcesso.istep = istep;
                            dadosExecucaoProcesso.tamBloco = tamBloco;
                            dadosExecucaoProcesso.wr = wr;
                            dadosExecucaoProcesso.wi = wi; 

                            //manda para todos os processos as informacoes de execucao
                            MPI_Bcast(&dadosExecucaoProcesso, 1, dadosExecucaoProcessoType, root, MPI_COMM_WORLD);
                
                        //Manda para os processo os blocos de trabalho (menos o root)
                        for (blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++) {
                            //printf("     qtdBlocos:%lu\n",blocoAtual);
                                					    
                            //calcula que parte do bloco que vai ser enviada
                            unsigned long inicioBloco = (((tamArray - 1)/qtdProcessos)*blocoAtual)+m;
                                					    
						    //manda para cada processo o seu bloco de processamento
						    MPI_Send(&data[inicioBloco], tamBloco, MPI_FLOAT, blocoAtual, 123, MPI_COMM_WORLD);
                            
                        }
                                               
                        //recebe dos processos os resultado do trabalho (menos o root)
                        for (blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++) {
                            //printf("     qtdBlocos:%lu\n",blocoAtual);
                            
                            //calcula que parte do bloco que vai ser recebida
                            unsigned long inicioBloco = (((tamArray - 1)/qtdProcessos)*blocoAtual)+m;
                                					    
						    //recebe de cada processo o resultado com seu bloco de processamento
						    //o que atualiza o vetor principal
   						    MPI_Recv(&data[inicioBloco], tamBloco, MPI_FLOAT, root, 123, MPI_COMM_WORLD,&status);
                        }                
                                
                  
				    } else {
				    	//se nao for necessario, calcula localmente
				        //printf("     processa na main\n");
				        calculoButterflyBloco(data,m,mmax,istep,wr,wi,((tamArray - 1)/qtdProcessos)+m);
				    }

                  wr=(wtemp=wr)*wpr-wi*wpi+wr;
                  wi=wi*wpr+wtemp*wpi+wi;
                }
                mmax=istep;
        //printf("<<< Final do while -> istep:%lu mmax:%lu\n\n",istep,mmax);
        }
        
        imprimeVetor(data);

        //liberando a memoria
  //      free(data);
        
	}else{
	
		//Todos os outros ja recebem no broadcast as informacoes do processo
	    //alocam o bloco float
	    dataLocal = malloc(sizeof(float) * dadosExecucaoProcesso.tamBloco);
	    
	    //recebe os dados
	    MPI_Recv(&dataLocal, dadosExecucaoProcesso.tamBloco, MPI_FLOAT, root, 123, MPI_COMM_WORLD,&status);
	
	    //processam o fft 
        calculoButterflyBloco(dataLocal,1,dadosExecucaoProcesso.mmax,dadosExecucaoProcesso.istep,dadosExecucaoProcesso.wr,dadosExecucaoProcesso.wi,dadosExecucaoProcesso.tamBloco);
	
	    //devolvem para o raiz o seu bloco de processamento
	    MPI_Send(&dataLocal, dadosExecucaoProcesso.tamBloco, MPI_FLOAT, rank, 123, MPI_COMM_WORLD);
	    
	    //desaloca o array temporario
	    free(dataLocal);
	
	}

    MPI_Type_free(&dadosExecucaoProcessoType);
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
    
     //wurmup
     //inicializaArray();
     //ordenaBitReverso();
     //fftMpi();

     //mede o tempo de execução da funcao f0 (media de NITER repeticoes) 
     gettimeofday(&inicio, NULL);
     RDTSC(tsc1);
        for(count=0;count<NUM_ITERACOES;count++){
             fftMpi(argc,argv);
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
    
    return 0;

}
