#include<stdio.h>
#include<stdlib.h>
#include <mpi.h>
#include <math.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr

//dados globais usados pelos processos
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
    int continua;
}DadosExecucaoProcesso;

DataTransporte *dataTransporte;
DadosExecucaoProcesso *dadosExecucaoProcesso;
MPI_Status status;
MPI_Datatype dataTransporteType,dadosExecucaoProcessoType;

MPI_Datatype oldtypesData[2],oldtypesExec[3];
int blockcountsData[2],blockcountsExec[3];
MPI_Aint offsetsData[2], extentData,offsetsExec[3], extentExec;

int rank,totalProcessos,qtdProcessos,root,pesoThreads;
unsigned long qtdElementos, tamArray, totalPeso;
float *data;

void imprimeVetor(float *dataTemp, unsigned long tamArrayTemp){
    int posicao;
    for(posicao=1;posicao<tamArrayTemp;posicao++){
        printf("%2.2f ",dataTemp[posicao]);
    }
    printf("\n\n");
}



void ordenaBitReverso(float *dataTemp, unsigned long tamArrayTemp){
    
    unsigned long i,j,m;
    float tempr;
    unsigned long qtdElementosTemp = (tamArrayTemp-1)/2;
    j=1;
    
    
    for (i=1;i<tamArrayTemp;i+=2) {
        if (j > i) {
            SWAP(dataTemp[j],dataTemp[i]);
            SWAP(dataTemp[j+1],dataTemp[i+1]);
        }
        m=qtdElementosTemp;
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

void inicializaMPI(int argc, char *argv[]){
    
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    root=totalProcessos-1;
    
    //printf("Iniciando processo %d de %d\n",rank,totalProcessos);
    
    //################ INICIALIZANDO ESTRUTURA TRANSPORTE DE DADOS #############
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
    
    //################ INICIALIZANDO ESTRUTURA INFORMACAO EXECUCAO #############
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

    MPI_Type_extent(MPI_DOUBLE, &extentExec);
    offsetsExec[2] = offsetsExec[1] + (2 * extentExec);
    oldtypesExec[2] = MPI_INT;
    blockcountsExec[2] = 1;        
    
    //Agora define o tipo estruturado e commita
    MPI_Type_struct(3, blockcountsExec, offsetsExec, oldtypesExec, &dadosExecucaoProcessoType);
    MPI_Type_commit(&dadosExecucaoProcessoType);	


}

void finalizaMPI(){
    MPI_Finalize();
}

void inicializaDadosExecucao(int argc, char *argv[]){
    
    if(argc < 3) {
        if (rank==root) {
            printf("ERRO: informe a quantidade de elementos do FFT, o peso das threads:: <./fft-mpi> <qtdElementos> <pesoThreads>\n");
        }
        finalizaMPI();
        exit(-1);
    }
    
    //inicializando valores do algoritmo
    qtdProcessos=root;
    qtdElementos = strtoul(argv[1],NULL,10);
    tamArray = qtdElementos*2 + 1;
    pesoThreads = atoi(argv[2]);
    totalPeso = (pow(qtdProcessos,pesoThreads));
    
    //printf("Dados de execucao inicializados: quantidade de elementos: %lu, tamArray: %lu, pesoThreads %d, totalPeso %lu, no processo: %d\n",qtdElementos,tamArray,pesoThreads,totalPeso,rank);
    
}

void inicializaArrayPrincipal(){

    //alocando memoria para o vetor de dados
    data = malloc(sizeof(float) * tamArray);
    
    //inicializa o array
    inicializaArray(data,tamArray);
    //e ordena o bit reverso
    ordenaBitReverso(data,tamArray);

}

void liberaArrayPrincipal(){
    free(data);
}

void fftMPI(){

    if(rank==root){
        //printf("Eu sou o processo root, o de numero: %d\n",root);
        //printf("Dados de execucao inicializados: quantidade de elementos: %lu, tamArray: %lu, pesoThreads %d, totalPeso %lu, no processo: %d\n",qtdElementos,tamArray,pesoThreads,totalPeso,rank);
        
        inicializaArrayPrincipal();
        
        //preenche os dados para enviar para os processos
        dadosExecucaoProcesso = malloc(sizeof(DadosExecucaoProcesso));
        dadosExecucaoProcesso[0].tamBloco = 4;
        dadosExecucaoProcesso[0].wr = 5.3;
        dadosExecucaoProcesso[0].wi = 7.2;
        dadosExecucaoProcesso[0].continua = 7;
        
        printf(" send tamBloco %lu, wr %f, wi %f \n",dadosExecucaoProcesso[0].tamBloco,dadosExecucaoProcesso[0].wr,dadosExecucaoProcesso[0].wi);

        //manda para todos os processos as informacoes de execucao
        MPI_Send(dadosExecucaoProcesso, 1, dadosExecucaoProcessoType,0,123, MPI_COMM_WORLD);
        
        //envia dados para os slaves
        dataTransporte = malloc(sizeof(DataTransporte) * 4);
        
        dataTransporte[0].index=1;
        dataTransporte[0].value=11;
        dataTransporte[1].index=2;
        dataTransporte[1].value=12;
        dataTransporte[2].index=3;
        dataTransporte[2].value=13;
        dataTransporte[3].index=4;
        dataTransporte[3].value=14;
        
        MPI_Send(dataTransporte, 4, dataTransporteType, 0, 123, MPI_COMM_WORLD);
        
        //imprimeVetor(data,tamArray);
        
        free(dadosExecucaoProcesso);
        free(dataTransporte);
        liberaArrayPrincipal();
        
    }else{
        //printf("Eu sou um processo slave, o de numero: %d\n",rank);
        //printf("Dados de execucao inicializados: quantidade de elementos: %lu, tamArray: %lu, pesoThreads %d, totalPeso %lu, no processo: %d\n",qtdElementos,tamArray,pesoThreads,totalPeso,rank);
        
        //recebe os dados de execucao do root
        dadosExecucaoProcesso = malloc(sizeof(DadosExecucaoProcesso));
        MPI_Recv(dadosExecucaoProcesso, 1, dadosExecucaoProcessoType, root, 123, MPI_COMM_WORLD,&status);
        
        //imprime os dados do broadcast
        printf("receive continua: %d tamBloco %lu, wr %f, wi %f \n",dadosExecucaoProcesso[0].continua,dadosExecucaoProcesso[0].tamBloco,dadosExecucaoProcesso[0].wr,dadosExecucaoProcesso[0].wi);
        
        
        //recebe dados do root
        dataTransporte = malloc(sizeof(DataTransporte) * 4);
        
        //recebe os dados
        MPI_Recv(dataTransporte, 4, dataTransporteType, root, 123, MPI_COMM_WORLD,&status);

        //imprime os dados
        printf("indice %lu, valor %f\n",dataTransporte[0].index,dataTransporte[0].value);
        printf("indice %lu, valor %f\n",dataTransporte[1].index,dataTransporte[1].value);
        printf("indice %lu, valor %f\n",dataTransporte[2].index,dataTransporte[2].value);
        printf("indice %lu, valor %f\n",dataTransporte[3].index,dataTransporte[3].value);
        
        
        free(dadosExecucaoProcesso);
        free(dataTransporte);
        
        
    }
    
}


int main(int argc, char *argv[]) {
    
    inicializaMPI(argc,argv);
    
    inicializaDadosExecucao(argc,argv);
    
    fftMPI();
    
    finalizaMPI();
    
    return 0;
    
}
