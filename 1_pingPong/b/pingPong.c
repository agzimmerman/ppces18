#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Maximum array size 2^10 = 1024 elements
#define MAX_ARRAY_SIZE (1<<10)

int main(int argc, char **argv)
{
    // Variables for the process rank and number of processes
    int myRank, numProcs;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    // Find out MPI communicator size and process rank
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    // PART B
    srandom(MPI_Wtime()*100000 + myRank*137);
    int numberOfElementsToSend = random() % 100;
    // Allocate an array big enough to hold event the largest message
    int *myArray = (int *)malloc(sizeof(int)*MAX_ARRAY_SIZE);
    if (myArray == NULL)
    {
        printf("Not enough memory\n");
        exit(1);
    }
    int numberOfElementsReceived;

    // Have only the first process execute the following code
    if (myRank == 0)
    {
        printf("Sending %i elements\n", numberOfElementsToSend);
        // TODO: Send "numberOfElementsToSend" elements

        // TODO: Receive elements

        // TODO: Store number of elements received in numberOfElementsReceived

        printf("Received %i elements\n", numberOfElementsReceived);
    }
    else // myRank == 1
    {
        // TODO: Receive elements

        // TODO: Store number of elements received in numberOfElementsReceived

        printf("Received %i elements\n", numberOfElementsReceived);

        printf("Sending back %i elements\n", numberOfElementsToSend);
        // TODO: Send "numberOfElementsToSend" elements

    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
