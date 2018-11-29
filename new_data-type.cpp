#include<stdio.h>
#include"mpi.h"
#include<stdlib.h>

int main(int argc,char **argv)
{
  int rank,proc_nums;
  MPI_Datatype arrays;
  MPI_Datatype matrix;
  int buffer[10];
  int vect_buffer[100];
  MPI_Status status;
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&proc_nums);

  MPI_Type_contiguous(10,MPI_INT,&arrays);
  MPI_Type_commit(&arrays);
  if(rank==0)
  {
    for(int i=0;i<10;i++){buffer[i]=i;}
    MPI_Send(buffer,1,arrays,1,0,MPI_COMM_WORLD);
  }
  else
  {
    MPI_Recv(buffer,1,arrays,0,0,MPI_COMM_WORLD,&status);
    for(int i=0;i<10;i++){printf("%d",buffer[i]);}
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Type_vector(10,1,9,arrays,&matrix);
  MPI_Type_commit(&matrix);

  if(rank==0)
  {
    printf("\n");
    for(int j=0;j<100;j++){buffer[j]=j;}
    MPI_Send(vect_buffer,1,matrix,1,0,MPI_COMM_WORLD);
  }
  else
  {
    MPI_Recv(vect_buffer,1,matrix,0,0,MPI_COMM_WORLD,&status);
    for(int j=0;j<100;j++){printf("%d",vect_buffer[j]);}
  }
  MPI_Finalize();
}
