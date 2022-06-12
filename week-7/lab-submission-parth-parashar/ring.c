//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// A simple demo program of MPI send-receive routines.
//
// - Process 0 sends an integer to process 1.
// - Process 1 decreases the integer's value by one, and sends it back
//
// Usage:
//   linux> mpirun -hostflie <hostfile> -n <#processes> simple [<N>]
//
//
#define _BSD_SOURCE
#include <unistd.h>     // for gethostname()
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define TAG 1001

int main(int argc, char *argv[])
{
  int nprocs, rank;
  char host[20];
  MPI_Status status;
  int N = 10;           // default integer value

  if (argc == 2) {
    N = atoi(argv[1]);  // overwirte the value
  }
  gethostname(host, 20);
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  if (nprocs < 2) {
    printf("Need at least 2 processes.\n");
    MPI_Finalize();
    return(1);
  }
  printf("P%d/%d started on %s ...\n", rank, nprocs, host);

    if(rank !=0)
    {
            MPI_Recv(&N,1, MPI_INT, rank-1,TAG, MPI_COMM_WORLD, &status);
            printf("Process %d received %d from process %d\n",rank,N,rank-1);
            N=N-1;
    }
    else
    {
            N = N-1;
    }

    MPI_Send(&N, 1, MPI_INT, (rank + 1)%nprocs, TAG, MPI_COMM_WORLD);
    printf("Process %d sent %d to process %d\n",rank, N,rank+1);

    //The next step is to take care of the fact that the last value must be handled by 0th process
    if(rank == 0)
    {
            MPI_Recv(&N,1, MPI_INT, rank-1, TAG, MPI_COMM_WORLD, &status);
             printf("Process %d received %d from process %d\n",rank,N,rank-1);
    }

  MPI_Finalize();
}