#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Maximum array size 2^10 = 1024 elements
#define MAX_ARRAY_SIZE (1<<10)

int main(int argc, char **argv)
{
    // Variables for the process rank and number of processes
    int myRank, numProcs;
    MPI_Status status;

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

    // PART C
    if (numProcs < 2)
    {
        printf("Error: Run the program with at least 2 MPI tasks!\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (myRank == 0)
    {
        printf("Rank %2.1i: Sending %i elements\n",
            myRank, numberOfElementsToSend);
        // Send "numberOfElementsToSend" elements
        MPI_Send(myArray, numberOfElementsToSend, MPI_INT, 1, 0,
            MPI_COMM_WORLD);
        // Receive elements
        // TODO: Determine the length of the messsage in advance
        numberOfElementsReceived = ...;

        MPI_Recv(myArray, numberOfElementsReceived, MPI_INT, 1, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Rank %2.1i: Received %i elements\n",
            myRank, numberOfElementsReceived);
    }
    else if (myRank == 1) // myRank == 1
    {
        // Receive elements
        // TODO: Determine the length of the message in advance
        numberOfElementsReceived = ...;

        MPI_Recv(myArray, numberOfElementsReceived, MPI_INT, 0, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Rank %2.1i: Received %i elements\n",
            myRank, numberOfElementsReceived);

        printf("Rank %2.1i: Sending back %i elements\n",
            myRank, numberOfElementsToSend);
        // Send "numberOfElementsToSend" elements
        MPI_Send(myArray, numberOfElementsToSend, MPI_INT, 0, 0,
            MPI_COMM_WORLD);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
