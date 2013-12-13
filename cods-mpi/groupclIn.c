#include "mpi.h"
#include <stdio.h>
#define NPROCS 8

int main(argc,argv)
int argc;
char *argv[];  
{
	int soma, rank, new_rank, sendbuf, recvbuf, numtasks,
	ranks[4]={5,3,6,7};
	MPI_Group  orig_group, new_group;
	MPI_Comm   new_comm;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if (numtasks != NPROCS) {
		printf("Must specify MP_PROCS= %d. Terminating.\n",NPROCS);
		MPI_Finalize();
      return 1;
	}
	
	/* Extract the original group handle */
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group);

	/* Cria um novo grupo */

	MPI_Group_incl(orig_group, 4, ranks, &new_group);
	/* Create new new communicator and then perform collective communications */
	MPI_Comm_create(MPI_COMM_WORLD, new_group, &new_comm);

	MPI_Group_rank(new_group, &new_rank);
	printf("rank= %d newrank= %d\n",rank,new_rank);

	if(new_comm != MPI_COMM_NULL){
		MPI_Reduce(&rank, &soma, 1, MPI_INT, MPI_SUM, 0, new_comm);
	}

	if(new_rank==0){
		printf("somatorio dos ids anteriores eh:%d\n",soma);
	}

	MPI_Finalize();
   return 0;
}
