#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "math.h"
#include "mpi.h"

int main(int argc,char** argv)
{
  int          taskid, ntasks;
  MPI_Status   status;
  int          ierr,i,j,itask;
  int            buffsize;
  double       *sendbuff,*recvbuff,buffsum,totalsum;
  double       inittime,totaltime;
  
 
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
  MPI_Comm_size(MPI_COMM_WORLD,&ntasks);

  buffsize=atoi(argv[1]);
  sendbuff=(double *)malloc(sizeof(double)*buffsize);
  recvbuff=(double *)malloc(sizeof(double)*buffsize);
  
  srand((unsigned)time( NULL ) + taskid);
  for(i=0;i<buffsize;i++){
      sendbuff[i]=(double)rand()/RAND_MAX;
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  
  buffsum=0.0;
  for(i=0;i<buffsize;i++){
    buffsum=buffsum+sendbuff[i];
  }
  printf(" Task %d : Sum of sendbuff elements = %e \n",taskid,buffsum);
    
  MPI_Barrier(MPI_COMM_WORLD);
  
  ierr=MPI_Reduce(&buffsum,&totalsum,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
  if(taskid==0){
    printf("                              TOTAL: %e \n\n",totalsum);   
  }
    
  inittime = MPI_Wtime();

  ierr=MPI_Reduce(sendbuff,recvbuff,buffsize,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
                  
  totaltime = MPI_Wtime() - inittime;
       
  if ( taskid == 0 ){
    buffsum=0.0;
    for(i=0;i<buffsize;i++){
      buffsum=buffsum+recvbuff[i];
    }
    printf(" Task %d : Sum of recvbuff elements -> %e \n",taskid,buffsum);
    printf(" Communication time : %f seconds\n\n",totaltime);  
  }

  free(recvbuff);
  free(sendbuff);

  MPI_Finalize();

}


