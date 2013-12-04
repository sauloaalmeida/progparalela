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
    int root = 0;
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == root) {
        printf ("Entre com o número de intervalos: \n");
        scanf("%d", &n);
    }
    
    MPI_Bcast(&n, 1, MPI_INT, root, MPI_COMM_WORLD);
    
    pi = calculaPi(n,rank,size);
    
    MPI_Reduce(&pi, &soma, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == root) {
        printf ("valor aproximado de pi: %.16f \n", soma);
    }
    MPI_Finalize();
    
}