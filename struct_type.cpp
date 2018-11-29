#include<stdio.h>
#include"mpi.h"
#include"stdlib.h"

#define oldtype_nums 3
#define INT_NUMS 3
#define DOUBLE_NUMS 4
#define CHAR_NUMS 5
#define contiguous_size 10

int main(int argc,char **argv)
{
  //定义数据类型，由3个int,4个double,5个char组成。
  struct my_data_type{
    int i[INT_NUMS];
    double d[DOUBLE_NUMS];
    char c[CHAR_NUMS];
  };
  //第二个新数据类型是上边数据类型的十个复制
  struct my_contiguous_data_type{
    my_data_type struct_data[contiguous_size];
  };

//第一个数据类型的生成

  int blocklens_array[oldtype_nums];
  MPI_Aint displs_array[oldtype_nums];//使用MPI_Aint，单个元素的存放空间可以放得下地址。
  MPI_Datatype old_type_array[oldtype_nums];

  MPI_Datatype mpi_data_type;
  MPI_Datatype mpi_data_type1;

  int myrank,proc_nums;
  MPI_Status status;

  int last_type_byte,gap_byte;
  MPI_Aint extent[2];

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  MPI_Comm_size(MPI_COMM_WORLD,&proc_nums);

  my_data_type mydata;

  old_type_array[0]=MPI_INT;
  old_type_array[1]=MPI_DOUBLE;
  old_type_array[2]=MPI_CHAR;
  blocklens_array[0]=INT_NUMS;
  blocklens_array[1]=DOUBLE_NUMS;
  blocklens_array[2]=CHAR_NUMS;

  //两种方式计算偏移
  /*
  //方式一：直接计算
  displs_array[0]=0;
  //计算int 类型和double类型之间的间隙
  last_type_byte=displs_array[0]+sizeof(int)*blocklens_array[0];
  gap_byte=sizeof(double)-last_type_byte%sizeof(double);
  displs_array[1]=last_type_byte+gap_byte;
  //char类型占1个byte，不用添加间隙
  last_type_byte=displs_array[1]+sizeof(double)*blocklens_array[1];
  displs_array[2]=last_type_byte;
  */
  //方式二：使用地址函数
  MPI_Address(&mydata.d[0],&displs_array[1]);//第一个double型相对于MPI_BOTTOM的偏移
  MPI_Address(&mydata.c[0],&displs_array[2]);//第一个char型相对于MPI_BOTTOM的偏移
  MPI_Address(&mydata.i[0],&displs_array[0]);
  displs_array[2]=displs_array[2]-displs_array[0];
  displs_array[1]=displs_array[1]-displs_array[0];
  displs_array[0]=0;
  //生成新的数据类型并提交
  MPI_Type_struct(oldtype_nums,blocklens_array,displs_array,old_type_array,&mpi_data_type);
  MPI_Type_commit(&mpi_data_type);

  if(myrank==0)
  {
    for(int i=0;i<INT_NUMS;i++){mydata.i[i]=i;}
    for(int j=0;j<DOUBLE_NUMS;j++){mydata.d[j]=double(j);}
    for(int k=0;k<CHAR_NUMS;k++){mydata.c[k]='a';}

    MPI_Send(&mydata.i[0],1,mpi_data_type,1,0,MPI_COMM_WORLD);
    printf("----------------------------------\n");
  }

  if(myrank==1)
  {
    MPI_Recv(&mydata.i[0],1,mpi_data_type,0,0,MPI_COMM_WORLD,&status);
    printf("-----------------------------------\n");
    for(int l=0;l<oldtype_nums;l++){
      printf("my i[%d] is %d \n",l,mydata.i[l]);
      printf("my d[%d] is %f \n",l,mydata.d[l]);
      printf("my c[%d] is %c \n",l,mydata.c[l]);
    }
  }

//把上边定义的数据类型重复十次作为新的数据类型
  my_contiguous_data_type con_data;
  MPI_Type_contiguous(contiguous_size,mpi_data_type,&mpi_data_type1);
  MPI_Type_commit(&mpi_data_type1);
  MPI_Type_extent(mpi_data_type,&extent[0]);
  MPI_Type_extent(mpi_data_type1,&extent[1]);
  printf("mpi_data_type extent is %d\n",extent[0]);
  printf("mpi_data_type1 extent is %d \n",extent[1]);

  if(myrank==0)
  {
    for(int o=0;o<contiguous_size;o++){
      for(int ii=0;ii<INT_NUMS;ii++){con_data.struct_data[o].i[ii]=ii;}
      for(int jj=0;jj<DOUBLE_NUMS;jj++){con_data.struct_data[o].d[jj]=double(jj);}
      for(int kk=0;kk<CHAR_NUMS;kk++){con_data.struct_data[o].c[kk]='a';}
    }

    MPI_Send(&con_data.struct_data[0].i[0],1,mpi_data_type1,1,0,MPI_COMM_WORLD);
  }

  if(myrank==1)
  {
    MPI_Recv(&con_data.struct_data[0].i[0],1,mpi_data_type1,0,0,MPI_COMM_WORLD,&status);
    printf("-----------------------------------\n");
    printf("%d\n",con_data.struct_data[1].i[1]);
  }

  MPI_Finalize();

  return 0;
}
