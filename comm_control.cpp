#include<stdio.h>
#include"mpi.h"
#include"stdlib.h"

int main(int argc,char** argv)
{
  int myrank,commsize1,commsize2;
  MPI_Group WORLD_GROUP,MY_GROUP;
  MPI_Comm my_comm;
  int ranges[1];
  int buff1,buff2;
  MPI_Init(&argc,&argv);
  MPI_Comm_group(MPI_COMM_WORLD,&WORLD_GROUP);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&commsize1);
  ranges[0]=0;
  MPI_Group_excl(WORLD_GROUP,1,ranges,&MY_GROUP);
  MPI_Comm_create(MPI_COMM_WORLD,MY_GROUP,&my_comm);
  if(myrank!=0)
  {
    MPI_Comm_size(my_comm,&commsize2);
    MPI_Reduce(&myrank,&buff1,commsize2,MPI_INT,MPI_SUM,1,my_comm);
  }
  MPI_Reduce(&myrank,&buff2,commsize1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
  //MPI_Comm_free(&my_comm);
  //MPI_Group_free(&WORLD_GROUP);
  //MPI_Group_free(&MY_GROUP);
  MPI_Finalize();
}
