#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <mpi.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define NUM_ITERACOES 1
#define ISIGN 1


//dados globais usados pelas threads
typedef struct strDataTransporte
{ 
    unsigned long index;
    float value;
}DataTransporte;

typedef struct strDadosExecucaoProcesso
{
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

void imprimeVetor(float *data, unsigned long tamArray){
    int posicao;
    for(posicao=1;posicao<tamArray;posicao++){
        printf("%2.2f ",data[posicao]);
    }
    printf("\n\n");
}

void imprimeVetorPackage(DataTransporte *data, unsigned long tamArray){
    int posicao;
    for(posicao=1;posicao<tamArray;posicao++){
        printf("\t\t%lu\t%2.2f\n ",data[posicao].index,data[posicao].value);
    }
    printf("\n\n");
}

void ordenaBitReverso(float *data, unsigned long tamArray){
    
    unsigned long i,j,m;
    float tempr;
    unsigned long qtdElementos = (tamArray-1)/2;
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

void inicializaArray(float *data, unsigned long tamArray){
    
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

void calculaButterflyMPI(DataTransporte *dataTransporte, double wr, double wi, unsigned long tamBloco){
    
    float tempr,tempi;
    unsigned long i, bloco;

    for (i=0;bloco<=tamBloco;i+=4) {
                                        
        //printf("          >>> FOR 2 -> wi:%f wr:%f istep:%lu mmax:%lu i:%lu j:%lu\n",wi,wr,istep, mmax, i, j);
        tempr=wr*dataTransporte[i+1].value-wi*dataTransporte[i+3].value;
        tempi=wr*dataTransporte[i+3].value+wi*dataTransporte[i+1].value;
        dataTransporte[i+1].value=dataTransporte[i].value-tempr;
        dataTransporte[i+3].value=dataTransporte[i+2].value-tempi;
        dataTransporte[i].value += tempr;
        dataTransporte[i+2].value += tempi; 
        bloco++;
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
    
    float *data;
    DataTransporte     *dataPackage;
    int rank;
    int qtdProcessos,totalProcessos,pesoThreads;
    unsigned long mmax,j,m,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;
    
    DataTransporte     dataTransporte;
    MPI_Datatype dataTransporteType, oldtypesData[2];
    int          blockcountsData[2];
    MPI_Aint    offsetsData[2], extentData;
    
    DadosExecucaoProcesso  dadosExecucaoProcesso;
    MPI_Datatype dadosExecucaoProcessoType, oldtypesExec[2];
    int          blockcountsExec[2];
    MPI_Aint    offsetsExec[2], extentExec;
    
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    
    
    //inicializacao do campo unsigned long
    offsetsData[0] = 0;
    oldtypesData[0] = MPI_UNSIGNED_LONG;
    blockcountsData[0] = 1;
    
    //inicializacao do campo float
    //precisa primero descobrir o offset pelo tamanho do MPI_UNSIGNED_LOG
    MPI_Type_extent(MPI_UNSIGNED_LONG, &extentData);
    offsetsData[1] = 1 * extentData;
    oldtypesData[1] = MPI_FLOAT;
    blockcountsData[1] = 1;    
    
    //Agora define o tipo estruturado e commita
    MPI_Type_struct(2, blockcountsData, offsetsData, oldtypesData, &dataTransporteType);
    MPI_Type_commit(&dataTransporteType);
    
    
    //inicializacao do campo unsigned long
    offsetsExec[0] = 0;
    oldtypesExec[0] = MPI_UNSIGNED_LONG;
    blockcountsExec[0] = 1;
    
    //inicializacao dos campos double
    //precisa primero descobrir o offset pelo tamanho do MPI_UNSIGNED_LOG
    MPI_Type_extent(MPI_UNSIGNED_LONG, &extentExec);
    offsetsExec[1] = 1 * extentExec;
    oldtypesExec[1] = MPI_DOUBLE;
    blockcountsExec[1] = 2;    
    
    //Agora define o tipo estruturado e commita
    MPI_Type_struct(2, blockcountsExec, offsetsExec, oldtypesExec, &dadosExecucaoProcessoType);
    MPI_Type_commit(&dadosExecucaoProcessoType);	
    
    
    int root = totalProcessos-1;
    qtdProcessos = root;
    
    //inicializando valores do algoritmo
    unsigned long qtdElementos, tamArray;
    qtdElementos = strtoul(argv[1],NULL,10);
    tamArray = qtdElementos*2 + 1;
    pesoThreads = atoi(argv[2]);
    unsigned long totalPeso = (pow(qtdProcessos,pesoThreads));
    
    if(rank==root){
        
        //alocando memoria para o vetor de dados
        data = malloc(sizeof(float) * tamArray);    
        
        //inicializa o array        
        inicializaArray(data,tamArray);
        //e ordena o bit reverso
        ordenaBitReverso(data,tamArray);
        
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
                    printf("     processa em thread\n");
                    unsigned long tamBloco = qtdElementos/mmax/qtdProcessos;
                    
                    printf("     tamBloco:%lu\n",tamBloco);
                    
                    //preenche os dados para enviar para os processos
                    dadosExecucaoProcesso = (DadosExecucaoProcesso) { .tamBloco = tamBloco, .wr = wr, .wi = wi };
                    
                    printf("     dados exeucao - tamBloco:%lu wr:%f wi:%f\n",dadosExecucaoProcesso.tamBloco,dadosExecucaoProcesso.wr, dadosExecucaoProcesso.wi);
                    
                    //manda para todos os processos as informacoes de execucao
                    MPI_Bcast(&dadosExecucaoProcesso, 1, dadosExecucaoProcessoType, root, MPI_COMM_WORLD);
                    
                    printf("envio o brodcast com os dados de execucao\n");
                    
                    //Manda para os processo os blocos de trabalho (menos o root)
                    for (blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++) {
                        printf("     blocoAtual:%lu\n",blocoAtual);
                        
                        //aloca a memoria do bloco que sera enviado
                        dataPackage = malloc(sizeof(DataTransporte) * tamBloco * 4);
                        printf("     alocou memoria para os dados que serao transportados\n");
                        
                        
                        //prepara o bloco que sera enviado
                        unsigned long count,bloco = 0;  
                        for (i=m;bloco<=tamBloco;i+=istep) {
                            j=i+mmax;                                
                            
                            //printf("dataPackage.index:%lu, datapackage.value:%f\n",i,data[i]);
                            
                            dataPackage[0].index=i;
                            dataPackage[0].value=data[i];

                            
                            printf("dataPackage.index:%lu, datapackage.value:%f\n",dataPackage[0].index,dataPackage[0].value );
                            
                            /*dataPackage[count+1]=&(DataTransporte){.index = j,.value = data[j]};
                            
                            dataPackage[count+2]=&(DataTransporte){.index = i+1,.value = data[i+1]};
                            
                            dataPackage[count+3]=&(DataTransporte){.index = j+1,.value = data[j+1]};*/
                            
                            bloco++;
                            count+=4;                                 
                        }
                        
                        imprimeVetorPackage(dataPackage,tamBloco*4);
     				    
                        //manda para cada processo o seu bloco de processamento
                        MPI_Send(&dataPackage, tamBloco*4, dataTransporteType, blocoAtual, 123, MPI_COMM_WORLD);
                        
                        //desaloca a memoria do bloco que sera enviado
                        free(dataPackage);
                        
                    }
                    
                    //recebe dos processos os resultado do trabalho (menos o root)
                    for (blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++) {
                        //printf("     qtdBlocos:%lu\n",blocoAtual);
                        
                        //aloca a memoria do bloco que sera recebido
                        dataPackage = malloc(sizeof(DataTransporte) * tamBloco * 4);
                        
                        //recebe de cada processo o resultado com seu bloco de processamento o que atualiza o vetor principal
                        MPI_Recv(&dataPackage, tamBloco*4, dataTransporteType, root, 123, MPI_COMM_WORLD,&status);
                        
                        //atualiza o vetor principal
                        unsigned long count2;
                        for(count2=0;count2<tamBloco*4;count2++){
                            data[dataPackage[count2].index]=dataPackage[count2].value;
                        }
                        
                        //desaloca o pacote de dados
                        free(dataPackage);
                        
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
        
        imprimeVetor(data,tamArray);
        
        //liberando a memoria
        free(data);
        
    }else{
     	
        //Todos os outros ja recebem no broadcast as informacoes do processo
        //alocam o bloco DataTransporte
        dataPackage = malloc(sizeof(DataTransporte) * dadosExecucaoProcesso.tamBloco*4);
        
        //recebe os dados
        MPI_Recv(&dataPackage, dadosExecucaoProcesso.tamBloco*4, dataTransporteType, root, 123, MPI_COMM_WORLD,&status);
     	
        //processam o fft 
        calculaButterflyMPI(dataPackage,dadosExecucaoProcesso.wr,dadosExecucaoProcesso.wi,dadosExecucaoProcesso.tamBloco);
     	
        //devolvem para o raiz o seu bloco de processamento
        MPI_Send(&dataPackage, dadosExecucaoProcesso.tamBloco, dataTransporteType, rank, 123, MPI_COMM_WORLD);
        
        //desaloca o array temporario
        free(dataPackage);
     	
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
    
    //wurmup
    //inicializaArray();
    //ordenaBitReverso();
    //fftMpi();
    
    //mede o tempo de execução da funcao f0 (media de NITER repeticoes) 
    gettimeofday(&inicio, NULL);
    RDTSC(tsc1);
    for(count=0;count<NUM_ITERACOES;count++){
        fftMpi(argc, argv);
    }
    
    RDTSC(tsc2);
    gettimeofday(&fim, NULL);
    tempo = (fim.tv_sec - inicio.tv_sec)*1000 + (fim.tv_usec - inicio.tv_usec)/1000; //calcula tempo em milisegundos
    //printf("tempo:%lf",tempo);
    clock = tsc2.int64 - tsc1.int64; //calcula numero de ciclos de CPU gastos
    //printf("Tempo FFT : %.1lf(ms) Clocks: %.2e \n", tempo/NUM_ITERACOES, (double)clock/NUM_ITERACOES);
    //printf("Clock/tempo: %.2e\n", clock/tempo);
    //printf("QtdElementos:%lu Threads: %d Peso:%d\n",qtdElementos,qtdProcessos,pesoThreads);
//printf("%lu\t%d\t%d\t%.1lf\t%.2e\t%.2e\n",qtdElementos,qtdProcessos,pesoThreads,tempo/NUM_ITERACOES,(double)clock/NUM_ITERACOES,clock/tempo);
    //imprimeVetor();
    
    return 0;
    
}
