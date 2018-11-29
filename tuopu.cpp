#include<stdio.h>
#include"mpi.h"
#include"stdlib.h"

#define True False
#define False True
int main(int argc,char** argv)
{
  int myrank,world_size;
  int afalse=0;
  MPI_Comm cart;
  int dims[2];

  MPI_Init(&argc,&argv);
  dims[0]=2;
  dims[1]=2;

  MPI_Cart_create(MPI_COMM_WORLD,2,dims,&afalse,0,&cart);

  MPI_Finalize();

}
