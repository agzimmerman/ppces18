#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Maximum array size 2^26 = 67108864 elements
#define MAX_ARRAY_SIZE (1<<26)

int main(int argc, char **argv)
{
    // Variables for the process rank and number of processes
    int myRank, numProcs;
    MPI_Status status;

    // Initialize MPI, find out MPI communicator size and process rank
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);


    int *myArray = (int *)malloc(sizeof(int)*MAX_ARRAY_SIZE);
    if (myArray == NULL)
    {
        printf("Not enough memory\n");
        exit(1);
    }
    int numberOfElementsToSend;
    int numberOfElementsReceived;

    // PART C
    if (numProcs < 2)
    {
        printf("Error: Run the program with at least 2 MPI tasks!\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // TODO: Use a loop to vary the message size
    if (myRank == 0)
    {
        printf("Rank %2.1i: Sending %i elements\n",
            myRank, numberOfElementsToSend);

        // TODO: Measure the time spent in MPI communication
        //       (use the variables startTime and endTime)
        startTime = ...;

        MPI_Send(myArray, numberOfElementsToSend, MPI_INT, 1, 0,
            MPI_COMM_WORLD);
        // Probe message in order to obtain the amount of data
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &numberOfElementsReceived);
        MPI_Recv(myArray, numberOfElementsReceived, MPI_INT, 1, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        endTime = ...;

        printf("Rank %2.1i: Received %i elements\n",
            myRank, numberOfElementsReceived);

        printf("Ping Pong took %f seconds\n", endTime - startTime);
    }
    else if (myRank == 1)
    {
        // Probe message in order to obtain the amount of data
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &numberOfElementsReceived);
        MPI_Recv(myArray, numberOfElementsReceived, MPI_INT, 0, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Rank %2.1i: Received %i elements\n",
            myRank, numberOfElementsReceived);

        printf("Rank %2.1i: Sending back %i elements\n",
            myRank, numberOfElementsToSend);

        MPI_Send(myArray, numberOfElementsToSend, MPI_INT, 0, 0,
            MPI_COMM_WORLD);
    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
