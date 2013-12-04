#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
   int rank, nprocs, i;
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   while( i<1000000)
      i++;
   printf("Hello, world.  I am %d of %d\n", rank, nprocs);
   MPI_Barrier(MPI_COMM_WORLD);
   printf("Hello, world.  I am %d of %d\n", rank, nprocs);
   MPI_Finalize();

   return 0;
} 
