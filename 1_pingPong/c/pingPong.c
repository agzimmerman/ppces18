#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Maximum array size 2^10 = 1024 elements
#define MAX_ARRAY_SIZE (1<<10)

int main(int argc, char **argv)
{
    // Variables for the process rank and number of processes
    int myRank, numProcs;

    // Initialize MPI, find out MPI communicator size and process rank
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    // PART B
    srandom(MPI_Wtime()*100000 + myRank*137);
    int numberOfElementsToSend = random() % 100;
    int *myArray = (int *)malloc(sizeof(int)*MAX_ARRAY_SIZE);
    if (myArray == NULL)
    {
        printf("Not enough memory\n");
        exit(1);
    }
    int numberOfElementsReceived;

    // TODO: Display an error message if too few processes

    // Have only the first process execute the following code
    if (myRank == 0)
    {
        printf("Sending %i elements\n", numberOfElementsToSend);
        // Send "numberOfElementsToSend" elements (ping)
        // -- first send a message with the number of elements
        MPI_Send(&numberOfElementsToSend, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        // -- send the elements themselves
        MPI_Send(myArray, numberOfElementsToSend, MPI_INT, 1, 0,
            MPI_COMM_WORLD);
        // Receive elements (pong)
        // Store number of elements received in numberOfElementsReceived
        // -- fist message brings the number of elements
        MPI_Recv(&numberOfElementsReceived, 1, MPI_INT, 1, 0, MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);
        // -- receive the elements themselves
        MPI_Recv(myArray, numberOfElementsReceived, MPI_INT, 1, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Received %i elements\n", numberOfElementsReceived);
    }
    // TODO: Make it work with more than two processes
    else // myRank == 1
    {
        // Receive elements (in two steps - see comments above)
        // Store number of elements received in numberOfElementsReceived
        MPI_Recv(&numberOfElementsReceived, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);
        MPI_Recv(myArray, numberOfElementsReceived, MPI_INT, 0, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Received %i elements\n", numberOfElementsReceived);

        printf("Sending back %i elements\n", numberOfElementsToSend);
        // Send "numberOfElementsToSend" elements
        MPI_Send(&numberOfElementsToSend, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(myArray, numberOfElementsToSend, MPI_INT, 0, 0,
            MPI_COMM_WORLD);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
