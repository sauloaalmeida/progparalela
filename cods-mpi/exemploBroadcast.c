#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
   int rank, nprocs, root = 0, i;
   int array[2];

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   if( rank == root)
   {
      for( i =0; i<2;i++)
      {
         array[i] = i*10;
      }
   }
   MPI_Bcast( array, 2, MPI_INT, root, MPI_COMM_WORLD);

   if( rank != root)
   {
      for( i =0;i<2; i++)
      {
         printf(" I am %d , array[%d] = %d\n", rank, i, array[i]);
      }
   }
   MPI_Finalize();
   return 0; 
} 
