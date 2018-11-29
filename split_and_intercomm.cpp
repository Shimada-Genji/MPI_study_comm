#include<stdio.h>
#include"mpi.h"
#include"stdlib.h"

int main(int argc,char** argv)
{
  int myrank,world_size;
  int my_split_size;
  MPI_Comm mycomm,intercomm;
  MPI_Status status;
  int split_flag;
  int reduce_result_value;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);

  //实现奇偶进程的分别在各自通信域reduce。
  split_flag=(myrank%2==0)?0:1;
  MPI_Comm_split(MPI_COMM_WORLD,split_flag,myrank,&mycomm);
  MPI_Reduce(&myrank,&reduce_result_value,1,MPI_INT,MPI_SUM,0,mycomm);
  if(myrank==1) printf("MY_COMM_ODD reduce result is %d \n",reduce_result_value);
  if(myrank==0) printf("MY_COMM_NO reduce result is %d \n",reduce_result_value);

  //组间通信域
  if(split_flag==0){
    MPI_Intercomm_create(mycomm,0,MPI_COMM_WORLD,1,1,&intercomm);
  }
  if(split_flag==1){
    MPI_Intercomm_create(mycomm,0,MPI_COMM_WORLD,0,1,&intercomm);
  }
  if(myrank==0){
    MPI_Send(&reduce_result_value,1,MPI_INT,0,0,intercomm);
  }
  if(myrank==1){
    MPI_Recv(&reduce_result_value,1,MPI_INT,0,0,intercomm,&status);
  }
  if(myrank==1) printf("after intercommunication reduce result is %d \n",reduce_result_value);
  MPI_Finalize();
  return 0;
}
