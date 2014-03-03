#include<stdio.h>
#include<stdlib.h>
#include <mpi.h>
#include <math.h>
#define SWAP(a,b) tempr=a;a=b;b=tempr
#define ISIGN 1

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
int          blockcountsData[2],blockcountsExec[3];
MPI_Aint    offsetsData[2], extentData,offsetsExec[3], extentExec;

int rank,totalProcessos,qtdProcessos,root,pesoThreads;
unsigned long qtdElementos, tamArray, totalPeso;
float  *data;
int devePararSlaves = 1;

void imprimeVetor(float *dataTemp, unsigned long tamArrayTemp){
    int posicao;
    for(posicao=1;posicao<tamArrayTemp;posicao++){
        printf("%2.2f ",dataTemp[posicao]);
    }
    printf("\n\n");
}

void imprimeVetorPackage(DataTransporte *data, unsigned long tamArrayTemp){
    unsigned long posicao;
    for(posicao=0;posicao<tamArrayTemp;posicao++){
        printf("\t\t%lu\t%lu\t%2.2f\n ",posicao,data[posicao].index,data[posicao].value);
    }
    printf("\n\n");
}

void calculoButterflyBloco(float dataBloco[], unsigned long m,unsigned long mmax, unsigned long istep, double wr, double wi, unsigned long tamBloco){
    
    float tempr,tempi;
    unsigned long j,i,bloco;
    bloco=1;
    
    for (i=m;bloco<=tamBloco;i+=istep) {
        j=i+mmax;                                
        //printf("          >>> FOR 2 -> wi:%f wr:%f istep:%lu mmax:%lu i:%lu j:%lu\n",wi,wr,istep, mmax, i, j);
        tempr=wr*dataBloco[j]-wi*dataBloco[j+1];
        tempi=wr*dataBloco[j+1]+wi*dataBloco[j];
        dataBloco[j]=dataBloco[i]-tempr;
        dataBloco[j+1]=dataBloco[i+1]-tempi;
        dataBloco[i] += tempr;
        dataBloco[i+1] += tempi; 
        bloco++;
    }
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
    
    unsigned long mmax,j,m,istep,i=1;
    double wtemp,wr,wpr,wpi,wi,theta;

    if(rank==root){
        //printf("Eu sou o processo root, o de numero: %d\n",root);
        //printf("Dados de execucao inicializados:  quantidade de elementos: %lu, tamArray: %lu, pesoThreads %d, totalPeso %lu, no processo: %d\n",qtdElementos,tamArray,pesoThreads,totalPeso,rank);
        
        inicializaArrayPrincipal();


        mmax=2;
        while (qtdElementos >= mmax) {
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
                    printf("     processa em thread\n");
                    unsigned long tamBloco = qtdElementos/mmax/qtdProcessos;
                    
                    printf("     tamBloco:%lu\n",tamBloco);

                       //preenche os dados para enviar para os processos
                       dadosExecucaoProcesso = malloc(sizeof(DadosExecucaoProcesso));
                       dadosExecucaoProcesso[0].tamBloco = tamBloco;
                       dadosExecucaoProcesso[0].wr = wr;
                       dadosExecucaoProcesso[0].wi = wi;
                       dadosExecucaoProcesso[0].continua = 1;
                       
                       //manda para todos os processos as informacoes de execucao
                       for(blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++){
                            MPI_Send(dadosExecucaoProcesso, 1, dadosExecucaoProcessoType,blocoAtual,123, MPI_COMM_WORLD);   
                           printf(" processo root sent - continua:%d, tamBloco: %lu, wr: %f, wi: %f \n",dadosExecucaoProcesso[0].continua, dadosExecucaoProcesso[0].tamBloco,dadosExecucaoProcesso[0].wr,dadosExecucaoProcesso[0].wi);
                       }
                       //libera os dados de execucao
                       free(dadosExecucaoProcesso);
                    
                                           
                        //aloca a memoria do bloco que sera enviado
                        dataTransporte = malloc(sizeof(DataTransporte) * tamBloco * 4);
                        printf("     processo %d - alocou memoria para os dados que serao transportados\n",rank);
                 
                        //prepara o bloco que sera enviado
                        unsigned long count=0;
                        unsigned long bloco=0;
                        blocoAtual=0;
                        for (i=m;i<=qtdElementos;i+=istep) {
                            j=i+mmax;                                
                            
                            //printf("dataPackage.index:%lu, datapackage.value:%f\n",i,data[i]);

                            dataTransporte[count].index=i;
                            dataTransporte[count].value=data[i];

                            dataTransporte[count+1].index = j;
                            dataTransporte[count+1].value = data[j];
                            
                            dataTransporte[count+2].index = i+1;
                            dataTransporte[count+2].value = data[i+1];
                            
                            dataTransporte[count+3].index = j+1;
                            dataTransporte[count+3].value = data[j+1];
                            
                            bloco++;
                            count+=4;
                            
                            if(bloco==tamBloco){
                                //envia os dados
                                printf("#########################  Imprimindo pacote antes de enviar \n");
                                imprimeVetorPackage(dataTransporte,tamBloco*4);
                                
                                count=0;
                                bloco=0;
                                //manda para cada processo o seu bloco de processamento
                                MPI_Send(dataTransporte, tamBloco*4, dataTransporteType, blocoAtual, 123, MPI_COMM_WORLD);
                                blocoAtual++;
                                
                            }
                        }
                    
                        printf("ultimo loop de envio, liberando memoria\n");
                        free(dataTransporte);
                     
                    
                    //MPI_Barrier(MPI_COMM_WORLD);
                    
                    
                    /*
                    //recebe dos processos os resultado do trabalho (menos o root)
                    for (blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++) {
                        //printf("     qtdBlocos:%lu\n",blocoAtual);
                        
                        //aloca a memoria do bloco que sera recebido
                        dataTransporte = malloc(sizeof(DataTransporte) * tamBloco * 4);
                        
                        //recebe de cada processo o resultado com seu bloco de processamento o que atualiza o vetor principal
                        //MPI_Recv(dataTransporte, tamBloco*4, dataTransporteType, root, 123, MPI_COMM_WORLD,&status);
                        
                        //atualiza o vetor principal
                        unsigned long count2;
                        //for(count2=0;count2<tamBloco*4;count2++){
                        //    data[dataTransporte[count2].index]=dataTransporte[count2].value;
                        //}
                        
                        //desaloca o pacote de dados
                        free(dataTransporte);
                        
                    } 
                     */
                

                    
                } else {
                    //se chegou aqui, na primeira vez, avisa para os slaves pararem
                    if(devePararSlaves==0){
                         
                       //preenche os dados para enviar para os processos
                       dadosExecucaoProcesso = malloc(sizeof(DadosExecucaoProcesso));
                       dadosExecucaoProcesso[0].tamBloco = 0;
                       dadosExecucaoProcesso[0].wr = 0;
                       dadosExecucaoProcesso[0].wi = 0;
                       dadosExecucaoProcesso[0].continua = 0;
                       
                       //manda para todos os processos as informacoes de execucao
                       for(blocoAtual=0; blocoAtual<qtdProcessos; blocoAtual++){
                            MPI_Send(dadosExecucaoProcesso, 1, dadosExecucaoProcessoType,blocoAtual,123, MPI_COMM_WORLD);   
                           printf(" processo root mandando parar slaves - continua:%d, tamBloco: %lu, wr: %f, wi: %f \n",
                                   dadosExecucaoProcesso[0].continua, 
                                   dadosExecucaoProcesso[0].tamBloco,
                                   dadosExecucaoProcesso[0].wr,
                                   dadosExecucaoProcesso[0].wi);
                       }
                       //libera os dados de execucao
                       free(dadosExecucaoProcesso);
                         
                       devePararSlaves=0;                         
                    }
                     
                    //e calcula localmente
                    printf("     processa na main\n");

                    calculoButterflyBloco(data,m,mmax,istep,wr,wi,qtdElementos/mmax);
                    //imprimeVetor(data,tamArray);
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
          while(1){
               
             //recebe os dados de execucao do root
             dadosExecucaoProcesso = malloc(sizeof(DadosExecucaoProcesso));
             MPI_Recv(dadosExecucaoProcesso, 1, dadosExecucaoProcessoType, root, 123, MPI_COMM_WORLD, &status);

             //imprime os dados do receive
             printf("processo %d - receive - continua: %d, tamBloco: %lu, wr %f, wi %f \n",
                         rank, 
                         dadosExecucaoProcesso[0].continua, 
                         dadosExecucaoProcesso[0].tamBloco,
                         dadosExecucaoProcesso[0].wr,
                         dadosExecucaoProcesso[0].wi);

             //caso tenham terminados os blocos
             if(dadosExecucaoProcesso[0].continua==0){
                break;
             }

             //senao recebe e processa os dados do root
              unsigned long packageSize=dadosExecucaoProcesso[0].tamBloco * 4;
             dataTransporte = malloc(sizeof(DataTransporte) * packageSize);
             MPI_Recv(dataTransporte, packageSize, dataTransporteType, root, 123, MPI_COMM_WORLD, &status);

             //imprime os dados
             printf("#########################  Imprimindo pacote recebido \n");
             imprimeVetorPackage(dataTransporte,packageSize);
             
             free(dadosExecucaoProcesso);
             free(dataTransporte);
               
          }                
        
    }
    
}


int main(int argc, char *argv[]) {
    
    inicializaMPI(argc,argv);
    
    inicializaDadosExecucao(argc,argv);
    
    fftMPI();
    
    finalizaMPI();
    
    return 0;
    
}
