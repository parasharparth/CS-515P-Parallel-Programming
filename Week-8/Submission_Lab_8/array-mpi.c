#include <stdio.h>
#include <mpi.h>

int compute(int i) { return i*i; }

int main(int argc, char **argv) {
  int rank, size, buf[2];
    MPI_File fh;  //input file
    MPI_File fh1; //output file
    MPI_Status st;
    int P = 2;
    int i=0;
    int cnt = 4;
    char fname[20];
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc < 2)
  {
        if (rank == 0)
        printf("Usage: mpirun -n <#procs> array-mpi <filename>\n");
        MPI_Finalize();
        return 0;
  }

  if(rank%P ==0)
  {
          printf("Succesful division of N by P");
          MPI_Offset fsize;
          MPI_File_get_size(fh, &fsize);
          // all processes open and read from the same file
             MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
               // read two integers
                 MPI_File_read(fh, buf, 2, MPI_INT, &st);
                   MPI_File_close(&fh);

                     printf("rank=%d: buf=[%d,%d]\n", rank, buf[0], buf[1]);
        // prepare data
                  int low = (cnt/size) * rank;    /* a simplistic partition scheme */
                    int high = low + (cnt/size);
                      int psum = 0;
                        for (int i = low; i < high; i++)
                                    psum += compute(i);

                          int sum=0, dest=0;
                            MPI_Reduce(&psum, &sum, 1, MPI_INT, MPI_SUM, dest, MPI_COMM_WORLD);
          // each process writes to a separate output file
             sprintf(fname, "%s.%d", argv[2], rank);
               MPI_File_open(MPI_COMM_SELF, fname, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &fh1);
                 // write four integers
                   MPI_File_write(fh1, buf, cnt, MPI_INT, &st);
                     MPI_File_close(&fh1);
  }
  else
  {
          printf("no division by P of N and exiting");
          MPI_Finalize();
  }
}
