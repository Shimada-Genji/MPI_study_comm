#include<stdio.h>
#include"mpi.h"
#include"stdlib.h"

int main(int argc,char** argv)
{
  int myrank,world_size,commsize1,commsize2;
  int ranksize;
  int value;
  int *ranks;
  MPI_Group WORLD_GROUP,MY_GROUP_ODD,MY_GROUP_NO;
  MPI_Comm MY_COMM_ODD,MY_COMM_NO;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&world_size);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_group(MPI_COMM_WORLD,&WORLD_GROUP);


  if(world_size%2==0) ranksize=world_size/2;
  else ranksize=world_size/2+1;
  ranks=(int *)malloc(ranksize*sizeof(int));
  for(int i=0;i<ranksize;i++){ranks[i]=2*i+1;}
  MPI_Group_excl(WORLD_GROUP,ranksize,ranks,&MY_GROUP_NO);
  MPI_Group_difference(WORLD_GROUP,MY_GROUP_NO,&MY_GROUP_ODD);
  MPI_Comm_create(MPI_COMM_WORLD,MY_GROUP_NO,&MY_COMM_NO);
  MPI_Comm_create(MPI_COMM_WORLD,MY_GROUP_ODD,&MY_COMM_ODD);

  if(myrank%2==1){
    MPI_Reduce(&myrank,&value,1,MPI_INT,MPI_SUM,0,MY_COMM_ODD);//需要注意此时进程1在新通信域中编号为0.
    MPI_Comm_free(&MY_COMM_ODD);
    MPI_Group_free(&MY_GROUP_ODD);
  }
  else{
    MPI_Reduce(&myrank,&value,1,MPI_INT,MPI_SUM,0,MY_COMM_NO);
    MPI_Comm_free(&MY_COMM_NO);
    MPI_Group_free(&MY_GROUP_NO);
  }
  if(myrank==1) printf("MY_COMM_ODD reduce result is %d \n",value);
  if(myrank==0) printf("MY_COMM_NO reduce result is %d \n",value);

  MPI_Finalize();
  return 0;
}
