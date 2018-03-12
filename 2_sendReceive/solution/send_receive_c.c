#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int maxElements = 1048576;

    int *data = (int *)malloc(sizeof(int) * maxElements);
    if (data == NULL)
    {
        printf("Not enough memory\n");
        return -1;
    }
    // Note: Data remains uninitialized, as this is just an example

    int myRank, numProcs;
    // MPI Initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if (numProcs != 2)
    {
        printf("This program can only be started with 2 MPI processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    int nextRank = (myRank + 1) % numProcs;
    int numElements;

    for (numElements = 4096; numElements < maxElements; numElements += 4096)
    {
        MPI_Status status;

        printf("Process %i sends and receives %i elements of data now\n",
            myRank, numElements);
        MPI_Sendrecv_replace(data, numElements, MPI_INT, nextRank, 0,
            nextRank, 0, MPI_COMM_WORLD, &status);
        printf("Process %i is done with %i elements of data\n",
            myRank, numElements);
    }

    MPI_Finalize();
    return 0;
}
