## Gather data

## install

Step1 install python3 for anaconda

Then install openmpi

Step2 used pip install mpi4py

## introduction 

The purpose of this task is to collect information about each process and output a matrix, which is arranged by rank id



## Main Code

```python
int main(int argc, char** argv) {

    int value = 0;
    int i;
    MPI_Status status;

  MPI_Init(NULL, NULL);

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  
  int a[3][5]={{0 , 1  ,2  ,3  ,4},  ## create a input data
              {5 , 6  ,7 , 8 , 9},
              {10, 11 ,12, 13,14}};

if(rank == 0) {
        
        std::vector<int> stat;
        int **a;
        a=(int**)malloc((size-1)*sizeof(int*));
        for(; ; ) {

            int receive_count = 0;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  #Detecting messages
            // printf("the probe source is %d\n",status.MPI_SOURCE);


                  stat.push_back(status.MPI_SOURCE);#Create an array, and each time a message is detected, check the number of messages and determine if the mpi_size is reached.
                  if(stat.size()<size-1){
                          // printf("the prossess %d is already \n",status.MPI_SOURCE);
                          MPI_Get_count(&status, MPI_INT, &receive_count);
                          int* number_buf = (int*)malloc(sizeof(int) * receive_count);
                          MPI_Recv(number_buf, receive_count, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);# recv array
                          for(int i = 0; i < sizeof(number_buf)-1; ++i)
                          {   
                              a[status.MPI_SOURCE]=number_buf;
                              printf("%d ", number_buf[i]);
                          }
                          printf("\n");

                  free(number_buf);
                  }else{
                          // printf("the prossess %d is already \n",status.MPI_SOURCE);
                          MPI_Get_count(&status, MPI_INT, &receive_count);
                          int* number_buf = (int*)malloc(sizeof(int) * receive_count);
                          MPI_Recv(number_buf, receive_count, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                          for(int i = 0; i < 5; ++i)
                          {
                              a[status.MPI_SOURCE]=number_buf;
                              printf("%d ", number_buf[i]);
                          }
                          printf("\n");
                          break;
                
            }
        }
        // int j;
        // for(int i = 0; i < 5; ++i)
        //   {   
        //       for (j=0;j<5;i++){
        //       printf("%d ", a[i][j]);
        //       }
        //   }
        //   printf("\n");

    } else {
        value = rank * 2;
        
        printf("send %d value***************",rank-1);
        
        for(int i = 0; i < 5; ++i)
        {
            printf("%d ", a[rank-1][i]);
        }
        printf("\n");
        MPI_Send(a[rank-1], sizeof(a[rank-1]), MPI_INT, 0, 99, MPI_COMM_WORLD); # every proesses send message to 0 poressage 
        
        
    }

    MPI_Finalize();
}


```

## output	

```shell
# soliva @ solivadeMacBook-Pro in ~/Desktop/Homework/MPI/MPI/task on git:main x [18:43:21]
$ mpirun -n 4 a.out
send 0 value***************0 1 2 3 4
send 1 value***************5 6 7 8 9
send 2 value***************10 11 12 13 14
[0] [1] [2] [3] [4]
[5] [6] [7] [8] [9]
[10] [11] [12] [13] [14]
```

