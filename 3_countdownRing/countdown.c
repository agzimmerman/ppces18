#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int random_dec(int current)
{
    int retVal = current - random()%10;
    return (retVal > 0) ? retVal : 0;
}

int main(int argc, char **argv)
{
    int myRank, numProcs;
    // TODO: Initialize MPI and obtain process identity

    // Rank-specific initialization of the random number generator
    srandom(MPI_Wtime() + myRank*100);

    int nextRank = (myRank + 1) % numProcs;
    int loserRank = -1;

    int countDownCounter = 50;

    // Rank 0 reads the initial timer value and starts the bomb
    if (myRank == 0)
    {
        if (argc > 1)
        {
            countDownCounter = atoi(argv[1]);
            if (countDownCounter <= 0) exit(1);
        }
        printf("Counting down from %i\n", countDownCounter);

        // TODO: Throw the bomb in

    }

    int done = 0;
    int data;

    // TODO: Implement a loop that runs until the counter hits 0
    {
        // TODO: Get the data from previous rank

        // Decrement the counter
        countDownCounter = random_dec(countDownCounter);
        if (countDownCounter > 0)
        {
            printf("Process %i has received the bomb (%i on the clock)"
                   " and is still alive!\n", myRank, countDownCounter);

            // TODO: Pass the bomb on

        }
        else
        {
            printf("Process %i lost\n", myRank);

            // TODO: Notify all processes of who the loser is

        }
    }
    printf("I am process %i and %i is the loser\n", myRank, loserRank);

    // TODO: Finalize MPI

    return 0;
}
