
#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;
int main(int argc, char **argv) {

  int i;
  char message[80];
  MPI_Status status;

  MPI_Init(NULL, NULL);

  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  //################### base code ##################

  if (rank == 0) {
    char **messages = new char *[size];

    for (int i = 1; i < size; i++) {

      int receive_count = 0;
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      MPI_Get_count(&status, MPI_CHAR, &receive_count);
      char *get_message = new char[receive_count + 1];

      MPI_Recv(get_message, receive_count, MPI_CHAR, MPI_ANY_SOURCE,
               MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      messages[rank - 1] = get_message;

      printf("recv message %s \n", get_message);
    }
  } else {

    sprintf(message, "Hello from procesor %i", rank);

    printf("send *******%s\n", message);
    MPI_Send(message, (int)(strlen(message)) + 1, MPI_CHAR, 0, 99,
             MPI_COMM_WORLD);
  }

  MPI_Finalize();
}
