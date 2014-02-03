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
}DadosExecucaoProcesso;

DataTransporte *dataTransporte;
DadosExecucaoProcesso *dadosExecucaoProcesso;
MPI_Status status;
MPI_Datatype dataTransporteType,dadosExecucaoProcessoType;

MPI_Datatype oldtypesData[2],oldtypesExec[2];
int          blockcountsData[2],blockcountsExec[2];
MPI_Aint    offsetsData[2], extentData,offsetsExec[2], extentExec;

int rank,totalProcessos,qtdProcessos,root,pesoThreads;
unsigned long qtdElementos, tamArray, totalPeso;
float  *data;

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
    
    //Agora define o tipo estruturado e commita
    MPI_Type_struct(2, blockcountsExec, offsetsExec, oldtypesExec, &dadosExecucaoProcessoType);
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
    
    //printf("Dados de execucao inicializados:  quantidade de elementos: %lu, tamArray: %lu, pesoThreads %d, totalPeso %lu, no processo: %d\n",qtdElementos,tamArray,pesoThreads,totalPeso,rank);
    
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
        //printf("Dados de execucao inicializados:  quantidade de elementos: %lu, tamArray: %lu, pesoThreads %d, totalPeso %lu, no processo: %d\n",qtdElementos,tamArray,pesoThreads,totalPeso,rank);
        
        inicializaArrayPrincipal();


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
                       dadosExecucaoProcesso = malloc(sizeof(DadosExecucaoProcesso));
                       dadosExecucaoProcesso[0].tamBloco = tamBloco;
                       dadosExecucaoProcesso[0].wr = wr;
                       dadosExecucaoProcesso[0].wi = wi;
                       
                       printf(" send tamBloco %lu, wr %f, wi %f \n",dadosExecucaoProcesso[0].tamBloco,dadosExecucaoProcesso[0].wr,dadosExecucaoProcesso[0].wi);
                       //manda para todos os processos as informacoes de execucao
                       for(blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++){                            MPI_Send(dadosExecucaoProcesso, 1, dadosExecucaoProcessoType,blocoAtual,123, MPI_COMM_WORLD);                                                   }
                       //libera os dados de execucao
                       free(dadosExecucaoProcesso);
                    
                    
                    //Manda para os processo os blocos de trabalho (menos o root)
                    for (blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++) {
                        printf("     blocoAtual:%lu\n",blocoAtual);
                        
                        //aloca a memoria do bloco que sera enviado
                        dataTransporte = malloc(sizeof(DataTransporte) * tamBloco * 4);
                        printf("     alocou memoria para os dados que serao transportados\n");
                 
                        //prepara o bloco que sera enviado
                        unsigned long count,bloco = 0;  
                        for (i=m;bloco<=tamBloco;i+=istep) {
                            j=i+mmax;                                
                            
                            //printf("dataPackage.index:%lu, datapackage.value:%f\n",i,data[i]);

                            dataTransporte[count].index=i;
                            dataTransporte[count].value=data[i];

                            dataTransporte[count+1].index = j;                            dataTransporte[count+1].value = data[j];
                            
                            dataTransporte[count+2].index = i+1;                            dataTransporte[count+2].value = data[i+1];
                            
                            dataTransporte[count+3].index = j+1;
                            dataTransporte[count+3].value = data[j+1];
                            
                            bloco++;
                            count+=4;                                 
                        }
                        
                        imprimeVetorPackage(dataTransporte,tamBloco*4);
     				    
                        //manda para cada processo o seu bloco de processamento
                        MPI_Send(dataTransporte, tamBloco*4, dataTransporteType, blocoAtual, 123, MPI_COMM_WORLD);
                        
                        //desaloca a memoria do bloco que sera enviado
                        free(dataTransporte);
                        
                    }
                    
                    //recebe dos processos os resultado do trabalho (menos o root)
                    for (blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++) {
                        //printf("     qtdBlocos:%lu\n",blocoAtual);
                        
                        //aloca a memoria do bloco que sera recebido
                        dataTransporte = malloc(sizeof(DataTransporte) * tamBloco * 4);
                        
                        //recebe de cada processo o resultado com seu bloco de processamento o que atualiza o vetor principal
                        MPI_Recv(&dataTransporte, tamBloco*4, dataTransporteType, root, 123, MPI_COMM_WORLD,&status);
                        
                        //atualiza o vetor principal
                        unsigned long count2;
                        for(count2=0;count2<tamBloco*4;count2++){
                            data[dataTransporte[count2].index]=dataTransporte[count2].value;
                        }
                        
                        //desaloca o pacote de dados
                        free(dataTransporte);
                        
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
        liberaArrayPrincipal();
        
    }else{
        //printf("Eu sou um processo slave, o de numero: %d\n",rank);
        //printf("Dados de execucao inicializados:  quantidade de elementos: %lu, tamArray: %lu, pesoThreads %d, totalPeso %lu, no processo: %d\n",qtdElementos,tamArray,pesoThreads,totalPeso,rank);
        
        //recebe os dados de execucao do root
        dadosExecucaoProcesso = malloc(sizeof(DadosExecucaoProcesso));
        MPI_Recv(dadosExecucaoProcesso, 1, dadosExecucaoProcessoType, root, 123, MPI_COMM_WORLD,&status);
        
        //imprime os dados do broadcast
        printf("receive tamBloco %lu, wr %f, wi %f \n",dadosExecucaoProcesso[0].tamBloco,dadosExecucaoProcesso[0].wr,dadosExecucaoProcesso[0].wi);
        
        
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
