#include <stdio.h>
#include <mpi.h>


double calculaPi(int n, int rank,int sizeLocal){
    
    int i;
    double piLocal, h, x, sum = 0.0;
    
    printf("calcula pi -> n:%d, rank:%d\n",n,rank);
    
    int inicio = rank*n;
	
    if (n != 0)
	{ 
        printf("Calcula pi -> Vai calcular pi\n");
        printf("Calcula pi -> inicio:%d fim:%d \n",inicio,inicio+n);
        
		h=1.0/(double) (n*sizeLocal);
		for(i=inicio+1; i <= inicio+n; i++)
		{ 
			x = h * ((double) i - 0.5);
			sum += (4.0/(1.0 + x*x));
		}
        
        piLocal=h * sum;
        
        printf("calcula pi: -> %f\n",piLocal);
        
		return piLocal;
    }
    
}


int main (int argc, char **argv){
    
    int rank, size, n, i;
    double pi, soma;
    MPI_Status status;
    int root = 0;
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == root) {
        printf ("Entre com o número de intervalos: \n");
        scanf("%d", &n);
        
        n=n/size;
        printf("n/%d = %d\n",size,n);
        
        for (i=1; i<size; i++) {
            //envia para cada processo a parte de pi que deve processar
            MPI_Send(&n, 1, MPI_INT, i, 123, MPI_COMM_WORLD);
            
        }
        
        //processa a parte dele de pi
        soma = calculaPi(n,rank,size);
        printf("resultado do processo 0 = %f\n",soma);
        
        for (i=1; i<size; i++) {

            //recebe de cada processo o resultado da soma
            MPI_Recv(&pi, 1, MPI_DOUBLE, i, 123, MPI_COMM_WORLD, &status);
            printf("resultado do processo:%d = %f\n",i,pi);
            
            //soma os resultados
            soma += pi;
            printf("Soma = %f\n",soma);
            
        }
        
        printf ("valor aproximado de pi: %.16f \n", soma);
    }
    
    if (rank !=root) {
        //recebe o valor
        MPI_Recv(&n, 1, MPI_INT, 0, 123, MPI_COMM_WORLD, &status);
        
        pi = calculaPi(n,rank,size);
        
        MPI_Send(&pi, 1, MPI_DOUBLE, 0, 123, MPI_COMM_WORLD);
    }
    
    
    MPI_Finalize();
    
}