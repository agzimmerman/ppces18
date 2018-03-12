#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* Timer still ticking, messages contains the timer value */
#define TAG_TICKER  1
/* Bomb has exploded, message contains the rank of the loser */
#define TAG_KABOOM  2

int random_dec(int current)
{
    int retVal = current - random()%10;
    return (retVal > 0) ? retVal : 0;
}

int main(int argc, char **argv)
{
    int myRank, numProcs;

    /* MPI Initialization */
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    /* Rank-specific initialization of the random number generator */
    srandom(MPI_Wtime() + myRank*100);

    int nextRank = (myRank + 1) % numProcs;
    int loserRank = -1;

    int countDownCounter = 50;

    /* Rank 0 starts the bomb ring */
    if (myRank == 0)
    {
        if (argc > 1)
        {
            countDownCounter = atoi(argv[1]);
            if (countDownCounter <= 0) exit(1);
        }
        printf("Counting down from %i\n", countDownCounter);
        // It is possible to also decrement the timer here
        MPI_Send(&countDownCounter, 1, MPI_INT, nextRank, TAG_TICKER,
            MPI_COMM_WORLD);
    }

    char myString[MPI_MAX_PROCESSOR_NAME];
    int myStringLength;

    MPI_Get_processor_name(myString, &myStringLength);
    printf("I am rank %i running on %s\n", myRank, myString);

    int done = 0;
    MPI_Request request = MPI_REQUEST_NULL;

    while (!done)
    {
        MPI_Status status;
        int data;

        MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
            MPI_COMM_WORLD, &status);
        if (status.MPI_TAG == TAG_KABOOM)
        {
            done = 1;
            loserRank = data;
        }
        else /* status.MPI_TAG == TAG_TICKER */
        {
            countDownCounter = random_dec(data);
            if (countDownCounter > 0)
            {
                printf("Process %i has received the bomb (%i on the clock)"
                       " and is still alive!\n", myRank, countDownCounter);
                MPI_Send(&countDownCounter, 1, MPI_INT, nextRank, TAG_TICKER,
                    MPI_COMM_WORLD);
            }
            else
            {
                int i;

                printf("Process %i lost\n", myRank);
                for (i = 0; i < numProcs; i++)
                {
                    if (i != myRank)
                        MPI_Send(&myRank, 1, MPI_INT, i, TAG_KABOOM,
                            MPI_COMM_WORLD);
                    else
                        // Make sure that send to self will not block
                        MPI_Isend(&myRank, 1, MPI_INT, i, TAG_KABOOM,
                            MPI_COMM_WORLD, &request);
                }
            }
        }
    }
    if (request != MPI_REQUEST_NULL)
        MPI_Wait(&request, MPI_STATUS_IGNORE);

    printf("I am process %i and %i is the loser\n", myRank, loserRank);

    MPI_Finalize();
    return 0;
}
