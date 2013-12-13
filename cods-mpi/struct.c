#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h" 
#include <sys/time.h>
#include <time.h>
#define TAM 2
#define MAX 65536

typedef  struct tempo
{
	struct timeval start, end;
}Tempo;

typedef struct {
	int numero;
	float vetor;
}VETOR_LOCAL;

inline void printTime( Tempo time);
void Build_derived_type(VETOR_LOCAL* vetor, MPI_Datatype* message_type_ptr);

int main ( int argc, char **argv)
{
	Tempo time;
	gettimeofday( &time.start,  NULL);
	int	myrank,	numprocs, tag = 50,	root = 0;
	MPI_Status status;

	MPI_Datatype pointMpi;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	//------------------------------------------------------------
	MPI_Datatype message_type;	
	VETOR_LOCAL vetor;
	if(myrank)
	{
		vetor.numero = MAX;
		vetor.vetor = 2.0;
	}
	Build_derived_type( &vetor, &message_type);
	if( myrank)
		MPI_Send( &vetor, 1, message_type, root, tag, MPI_COMM_WORLD);
	
	int i;
	if( !myrank)
	{
		MPI_Recv( &vetor, 1, message_type, 1, tag, MPI_COMM_WORLD, &status);
		printf(" vetor.numero = %d\n \n", vetor.numero);
		
			printf( " vetor.vetor = %f\n", vetor.vetor);		
	}
	//------------------------------------------------------------
	MPI_Finalize( );
	gettimeofday( &time.end,  NULL);
	//printTime(time);

	return 0;
}

inline void printTime( Tempo time)
{
	printf("Tempo utilizado = %ld microseconds \n", (time.end.tv_sec-time.start.tv_sec) * 1000000 +(time.end.tv_usec-time.start.tv_usec));
}


void Build_derived_type(VETOR_LOCAL* vetor, MPI_Datatype* message_type_ptr)
{
	int block_lengths[TAM];
	MPI_Aint displacements[TAM];
	MPI_Aint addresses[TAM+1];
	MPI_Datatype typelist[TAM];
	MPI_Datatype newType;

	//MPI_Type_contiguous( MAX, MPI_FLOAT, &newType);

	typelist[0] = MPI_INT;
	typelist[1] = MPI_FLOAT;

	block_lengths[0] = block_lengths[1] = 1;

	MPI_Address(vetor, &addresses[0]);
	MPI_Address(&(vetor->numero), &addresses[1]);
	MPI_Address(&(vetor->vetor), &addresses[2]);

	displacements[0] = addresses[1] - addresses[0];
	displacements[1] = addresses[2] - addresses[0];
	
	MPI_Type_struct(TAM, block_lengths, displacements, typelist, message_type_ptr);

	MPI_Type_commit(message_type_ptr);
}

