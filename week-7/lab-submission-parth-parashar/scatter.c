//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// MPI scatter routine.
//
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int rank, size;

  int *data;
  int *disp;
 int rbuf[2*size];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // process 0 prepares an array of data
  int *cnt;
  if (rank == 0) {
    cnt = (int *) malloc(sizeof(int) * size);
    data = (int *) malloc(sizeof(int) *128);
    disp = (int *) malloc(sizeof(int) *size);

    disp[0] = 0;
    disp[1] = 0;
    for (int i = 0; i < size; i++)
    {
      cnt[i] = 2 * i;
    data[i] = i;
    if(i>=2)
    {
            disp[i] = i*(i-1);
    }
    }
  }

  // scatter the data to all processes
  int mycnt;
  MPI_Scatter(cnt, 1, MPI_INT, &mycnt, 1, MPI_INT, 0, MPI_COMM_WORLD);
  printf("P[%d] mycnt = %d\n", rank, mycnt);

  MPI_Scatterv(data, cnt, disp, MPI_INT, rbuf, 2*size, MPI_INT, 0, MPI_COMM_WORLD);

  for(int i=0;i<size;i++)
  {
          printf("P[%d] got %d items\n",i,disp[i]);
  }
  MPI_Finalize();
}