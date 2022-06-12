#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

#define DATABITS 16     // assume data are 16-bit integers: [0,8191]
// return true if x is a power of 2
#define IsPowerOf2(x) (!((x) & ((x) - 1)))

// bucket index for integer x of b bits (B is #buckets)
#define BktIdx(x,b,B) ((x) >> ((b) - (int)log2(B)))
#define TAG 1001

void init_array(int *a, int n)
{
        for (int i = 0; i < n; i++)
        {
                a[i] = 0;
        }
}

int main(int argc, char **argv)
{
        //Step-1:- Initialize the MPI and get the value of P from the user.
        int rank,size;
        MPI_Init(&argc,&argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Status status;
        //printf("The rank of the process is: %d\n",rank);

        //Step-2:- check the value of P is power of 2.
        if (!IsPowerOf2(size)) {
        printf("P (#buckets) must be a power of 2\n");
        MPI_Finalize();
        exit(0);
        }


        //Step-3:- The next step is to open the file and get the file size
        MPI_File fh,fout;
        MPI_Status st;
        MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
        MPI_Offset fsize;
        MPI_File_get_size(fh, &fsize);

        //Step-4: - The next step is to calculate the data size from file size
        int N = fsize/sizeof(int);
        printf("the number of integers present in the file is: %d\n",N);
        int P = size;

        //Step-5:- The next step is to check that P divides N evenly
        if(N%P != 0)
        {
                printf("P does not divide N evenly");
                MPI_Finalize();
                exit(0);
        }

        //step-6:- declare an array of size N/P
        int a[N/P];
        init_array(a,N/P);

        //step-7:- To create a unique file-view offset
        int offset = rank * N/P * sizeof(int);
        printf("the offset value is: %d\n",offset);

        //step-8:- to read from the file
        MPI_File_set_view(fh, offset, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
        MPI_File_read(fh, a, N/P, MPI_INT, &st);

        //for (int i = 0; i < N/P; i++)
        //{
         //   printf("array is: %d\n",a[i]);
        //}


        //Step-9:- Calculate the value of array dimensions and then pass it to bucket sort
        int dimension1 = P;
        int dimension2 = 2*(N/P)/P;
        if(dimension2<4)
        {
                dimension2 = 4;
        }

        int bucket[dimension1][dimension2];

        //Step-10:- distribution of data into the buckets
        //individual bucket count
        int num_buckets = P;
        int bcnt[num_buckets];
        for (int k = 0; k < num_buckets; k++)
                bcnt[k] = 0;

        //distribute data to the buckets
        for (int i = 0; i < N/P; i++)
        {
                int k = BktIdx(a[i], DATABITS, num_buckets);
                bucket[k][bcnt[k]++] = a[i];
        }


        //Step-11:- Send bucket[k] to process k
        for(int k = 0; k < P; k++)
        MPI_Send(bucket,k,MPI_INT,k,TAG,MPI_COMM_WORLD);


        //Step-12:- Receive the buckets
        //using a temporary bucket to receive the bucket data and store it in the temporary bucket
        int buckettemparray[dimension1][dimension2];
        memcpy (buckettemparray, bucket, dimension1*dimension2*sizeof(int));
        int buckettemp[2*N/P];    //creating a temporary bucket to be merged during bubble sorting

        for(int k=0; k<P; k++)
        MPI_Recv(bucket,N,MPI_INT,k,TAG,MPI_COMM_WORLD,&st);

        //Step-13:- Merge the p-1 buckets with its own bucket and call bubble sort to sort the merged array

        for (int k = 0; k < num_buckets; k++)
        {
        for (int i = 0; i < N/P; i++)
        {
        for (int j = i+1; j < N/P; j++)
        {
        if (a[i] > a[j])
        {
                int tmp = a[i];
                a[i] = a[j];
                a[j] = tmp;
        }
        }
        }
        }


        printf("sorted array received for the rank %d\n",rank);
        //Step-15 printing the sorted array
        for(int i=0; i<N/P; i++)
        {
                printf("%d \n",a[i]);
        }

        //Step-14:- Writing the sorted array to the file
        int outputoffset = rank * bcnt[rank] * sizeof(int);
        MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &fout);
        MPI_File_set_view(fout, offset, MPI_INT, MPI_INT, "native", MPI_INFO_NULL);
        MPI_File_write(fout, a, offset, MPI_INT, &st);
        MPI_File_close(&fout);
        MPI_File_close(&fh);
        MPI_Finalize();
}