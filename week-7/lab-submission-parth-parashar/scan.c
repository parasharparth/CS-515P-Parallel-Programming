//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// MPI scan routine.
//
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // sum-scan the rank numbers
  int psum;
  int sum;
  MPI_Scan(&rank, &psum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  printf("P[%d] psum = %d\n", rank, psum);

  MPI_Gather(&sum, 1, MPI_INT, psum, 1, MPI_INT, rank, MPI_COMM_WORLD);
  printf("The total sum is:- %d\n",sum);
  MPI_Finalize();
}