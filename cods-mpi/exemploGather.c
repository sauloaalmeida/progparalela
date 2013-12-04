#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TAM 2
int main(int argc, char *argv[])
{
   int gsize,sendarray[TAM];
   int root, myrank, *rbuf, i;
   MPI_Init(&argc,&argv); 
   MPI_Comm_rank( MPI_COMM_WORLD, &myrank);
   if ( myrank == root) {
      MPI_Comm_size( MPI_COMM_WORLD, &gsize);
      rbuf = (int *)malloc(gsize*TAM*sizeof(int));
      }
   for( i=0; i<TAM; i++)
      sendarray[i] = (i+1) *(myrank+1);
   MPI_Gather( sendarray, TAM, MPI_INT, rbuf, TAM, MPI_INT, root, MPI_COMM_WORLD);
   
   if ( myrank == root) 
   {
      for( i=0; i<TAM*gsize; i++)
         printf(" array[%d] = %d\n", i, rbuf[i]);
   }

   MPI_Finalize();
   return 0; 
} 
