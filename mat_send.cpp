#include<stdio.h>
#include"mpi.h"
#include"stdlib.h"

#define mat_size 10

int main(int argc,char **argv)
{
  //定义矩阵，连续存放，使用mpi_type_vector生成新的数据类型来发送。
  struct vect{
    int vec[mat_size];
  };
  struct matrix{
    vect vecs[mat_size];
  };
  struct matrix3d{
    matrix matrixs[mat_size];
  };
  MPI_Datatype mpi_vec_type;
  MPI_Datatype mpi_3d_type;
  MPI_Aint extent[2];
  //使用mpi_pack打包的矩阵，存放位置不一定连续
  int matrix_for_pack[mat_size][mat_size][mat_size];
  int *buffer_for_packed;
  int buff_size;
  int position;//打包位置偏移

  int myrank,proc_nums;
  MPI_Status status;
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&proc_nums);
//定义并提交三维矩阵数据类型
  MPI_Type_contiguous(mat_size,MPI_INT,&mpi_vec_type);
  MPI_Type_commit(&mpi_vec_type);
  MPI_Type_vector(mat_size,mat_size,mat_size,mpi_vec_type,&mpi_3d_type);
  MPI_Type_commit(&mpi_3d_type);
//测试新数据类型
  MPI_Type_extent(mpi_vec_type,&extent[0]);
  MPI_Type_extent(mpi_3d_type,&extent[1]);
  matrix3d matrix_3d;
  if(myrank==0)
  {
    printf("mpi_vec_type extent is %d\n",extent[0]);
    printf("mpi_3d_type extent is %d\n",extent[1]);
    for(int i=0;i<mat_size;i++){
      for(int j=0;j<mat_size;j++){
        for(int k=0;k<mat_size;k++){matrix_3d.matrixs[i].vecs[j].vec[k]=1;}
      }
    }
  MPI_Send(&matrix_3d.matrixs[0].vecs[0].vec[0],1,mpi_3d_type,1,0,MPI_COMM_WORLD);
  }
  if(myrank==1)
  {
    MPI_Recv(&matrix_3d.matrixs[0].vecs[0].vec[0],1,mpi_3d_type,0,0,MPI_COMM_WORLD,&status);
    for(int i=0;i<mat_size;i++){
      for(int j=0;j<mat_size;j++){
        for(int k=0;k<mat_size;k++){
          //printf("matrix_3d[%d][%d][%d] is %d \n",i,j,k,matrix_3d.matrixs[i].vecs[j].vec[j]);}
      }
    }
  }
  //接下来使用MPI_PACK来打包不连续的矩阵
  MPI_Barrier(MPI_COMM_WORLD);
  buff_size=mat_size*mat_size*mat_size*sizeof(int);
  buffer_for_packed=(int *)malloc(buff_size);
  if(myrank==0){
    for(int l=0;l<mat_size;l++){
      for(int m=0;m<mat_size;m++){
        for(int n=0;n<mat_size;n++){matrix_for_pack[l][m][n]=l*100+m*10+n;}
      }
    }
    //打包到连续的缓冲区
    position=0;
    for(int ii=0;ii<mat_size;ii++){
      for(int jj=0;jj<mat_size;jj++){
        MPI_Pack(matrix_for_pack[ii][jj],mat_size,MPI_INT,buffer_for_packed,buff_size,&position,MPI_COMM_WORLD);
      }
    }
    MPI_Send(buffer_for_packed,buff_size,MPI_PACKED,1,0,MPI_COMM_WORLD);
    printf("matrix has been packed and send!\n");
  }
  if(myrank==1){
    MPI_Recv(buffer_for_packed,buff_size,MPI_PACKED,0,0,MPI_COMM_WORLD,&status);
    position=0;
    for(int ii=0;ii<mat_size;ii++){
      for(int jj=0;jj<mat_size;jj++){
        MPI_Unpack(buffer_for_packed,buff_size,&position,matrix_for_pack[ii][jj],mat_size,MPI_INT,MPI_COMM_WORLD);
      }
    }
    printf("recieved and unpacked!\n");
    for(int l=0;l<mat_size;l++){
      for(int m=0;m<mat_size;m++){
        for(int n=0;n<mat_size;n++){
          //printf("%d\n",matrix_for_pack[l][m][n]);
        }
      }
    }
  }
  MPI_Finalize();
  return 0;
}
