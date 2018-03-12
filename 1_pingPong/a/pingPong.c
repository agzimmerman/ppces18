#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

int main(int argc, char **argv)
{
    int ierr;
    int myRank, numProcs;
    int pingCount = 42;
    int pongCount = 0;
    int pinger = 0;
    int ponger = 1;
    int count = 1;
    int tag = 0;

    // Initialize MPI
    ierr = MPI_Init(&argc, &argv);


    // Find out MPI communicator size and process rank
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    
    assert(numProcs == 2);  // This is two-player ping-pong!
    
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    

    // Have only the first process execute the following code
    if (myRank == pinger)
    {
        // Send pingCount
        printf("Rank %u: Sending Ping (# %i)\n", myRank, pingCount);

        MPI_Send(&pingCount, count, MPI_INT, ponger, tag, MPI_COMM_WORLD);


        // Receive pongCount
        MPI_Recv(&pongCount, count, MPI_INT, ponger, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Rank %u: Received Pong (# %i)\n", myRank, pongCount);

        pingCount = 0 + pongCount;
    }
    // Do proper receive and send in any other process
    else
    {
        // Receive pingCount
        MPI_Recv(&pingCount, count, MPI_INT, pinger, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Rank %u: Received Ping (# %i)\n", myRank, pingCount);

        // calculate and send pongCount
        pongCount = 0 - pingCount;
        
        printf("Rank %u: Sending Pong (# %i)\n", myRank, pongCount);

        MPI_Send(&pongCount, count, MPI_INT, pinger, tag, MPI_COMM_WORLD);

    }

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
