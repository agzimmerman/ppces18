#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*********************************************************************
 * Broadcasts 'count' elements of 'data' from rank 'root' to all
 * other processes in the communicator 'comm'.
 *********************************************************************/
void bcast_int(int *data, int count, int root, MPI_Comm comm)
{
    int myRank, numProcs;
    MPI_Comm_size(comm, &numProcs);
    MPI_Comm_rank(comm, &myRank);

    if (myRank != root)
       MPI_Recv(data, count, MPI_INT, root, 0, comm, MPI_STATUS_IGNORE);
    else
    {
        int i;
        for (i = 0; i < numProcs; i++)
        {
            if (i != root)
                MPI_Send(data, count, MPI_INT, i, 0, comm);
        }
    }
}

/*********************************************************************
 * Scatters the content of 'sendbuf' in process with rank 'root' to
 * all processes in the communicator 'comm'. Each piece of data is
 * 'sendcnt' elements long. Received data goes into 'recvbuf'.
 *********************************************************************/
void scatter_int(int *sendbuf, int sendcnt, int *recvbuf, int recvcnt,
                 int root, MPI_Comm comm)
{
    int myRank, numProcs;
    MPI_Comm_size(comm, &numProcs);
    MPI_Comm_rank(comm, &myRank);

    if (myRank != root)
        MPI_Recv(recvbuf, recvcnt, MPI_INT, root, 0, comm, MPI_STATUS_IGNORE);
    else
    {
        int i;
        for (i = 0; i < numProcs; i++)
        {
            if (i != root)
                MPI_Send(sendbuf + i*sendcnt, sendcnt, MPI_INT, i, 0, comm);
        }
        memcpy(recvbuf, sendbuf + root*sendcnt, recvcnt*sizeof(int));
    }
}

/*********************************************************************
 * Gathers the content of each process' 'sendbuf' into the 'recvbuf'
 * of process with rank 'root'. Each process sends 'sendcount' pieces
 * of data and the root receives 'recvcnt' from each process.
 *********************************************************************/
void gather_int(int *sendbuf, int sendcnt, int *recvbuf, int recvcnt,
                int root, MPI_Comm comm)
{
    int myRank, numProcs;
    MPI_Comm_size(comm, &numProcs);
    MPI_Comm_rank(comm, &myRank);

    if (myRank != root)
        MPI_Send(sendbuf, sendcnt, MPI_INT, root, 0, comm);
    else
    {
        int i;
        for (i = 0; i < numProcs; i++)
        {
            if (i != root)
                MPI_Recv(recvbuf + i*recvcnt, recvcnt, MPI_INT, i, 0,
                    comm, MPI_STATUS_IGNORE);
        }
        memcpy(recvbuf + root*recvcnt, sendbuf, recvcnt*sizeof(int));
    }
}

/*********************************************************************
 * Scatters the content of each process' 'sendbuf' into the 'recvbuf'
 * of each other process. Received pieces must be ordered by their
 * sender's rank.
 *********************************************************************/
void alltoall_int(int *sendbuf, int sendcnt, int *recvbuf, int recvcnt,
                  MPI_Comm comm)
{
    int myRank, numProcs;
    MPI_Comm_size(comm, &numProcs);
    MPI_Comm_rank(comm, &myRank);

    int i, j;
    for (i = 0; i < numProcs; i++)
    {
        if (i == myRank)
        {
            for (j = 0; j < numProcs; j++)
            {
                if (i != j)
                    MPI_Send(sendbuf + j*sendcnt, sendcnt, MPI_INT, j, 0, comm);
            }
            memcpy(recvbuf + i*recvcnt, sendbuf + i*sendcnt,
                recvcnt*sizeof(int));
        }
        else
            MPI_Recv(recvbuf + i*recvcnt, recvcnt, MPI_INT, i, 0,
                comm, MPI_STATUS_IGNORE);
    }
}

/*********************************************************************
 * Performs a global summation reduction over the single integer value
 * in all processes' 'sendbuf'-s. The result should be available in
 * 'recvbuf' only at process with rank 'root'.
 *********************************************************************/
void sum_int(int *sendbuf, int *recvbuf, int root, MPI_Comm comm)
{
    int myRank, numProcs;
    MPI_Comm_size(comm, &numProcs);
    MPI_Comm_rank(comm, &myRank);

    if (myRank == root)
    {
        int i, sum = *sendbuf, temp;
        for (i = 0; i < numProcs-1; i++)
        {
            MPI_Recv(&temp, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm,
                MPI_STATUS_IGNORE);
            sum += temp;
        }
        *recvbuf = sum;
    }
    else
        MPI_Send(sendbuf, 1, MPI_INT, root, 0, comm);
}

#define INSPECT_RANK 0

int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);

    int myRank, numProcs;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    int i, myRandomNumber;
    int inspectRank = INSPECT_RANK;

    // Allocate data buffers
    int *mySendBuffer = (int *)malloc(sizeof(int)*numProcs);
    int *myRecvBuffer = (int *)malloc(sizeof(int)*numProcs);

    // Seed the random number generator with a rank-specific value
    srand(MPI_Wtime() + 123*myRank);
    myRandomNumber = random() % 100;

    if (argc > 1)
    {
        inspectRank = atoi(argv[1]);
        if (inspectRank < 0 || inspectRank >= numProcs)
            MPI_Abort(MPI_COMM_WORLD, 1);
    }
    printf("Rank %2.1i has the random number %i\n", myRank, myRandomNumber);

    int myData = -1;

    // Use MPI barrier to synchronize the output
    MPI_Barrier(MPI_COMM_WORLD);

    // ---- Broadcast operation ----

    if (myRank == inspectRank)
        printf("-------------------------------------\n"
               "broadcast\n"
               "-------------------------------------\n");

    // Prepare the local data
    myData = myRandomNumber;

    if (myRank == inspectRank)
        printf("Rank %2.1i:\tmyData before the broadcast is %i\n",
            inspectRank, myData);

    // Call the user's implementation of broadcast
    bcast_int(&myData, 1, 0, MPI_COMM_WORLD);
    if (myRank == inspectRank)
        printf("Rank %2.1i:\tmyData after the broadcast is %i\n",
            inspectRank, myData);

    MPI_Bcast(&myData, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (myRank == inspectRank)
        printf("Rank %2.1i:\tmyData after the MPI broadcast is %i\n",
            inspectRank, myData);

    MPI_Barrier(MPI_COMM_WORLD);

    // ---- Scatter operation ----

    if (myRank == inspectRank)
        printf("-------------------------------------\n"
               "scatter\n"
               "-------------------------------------\n");

    // Prepare the local data. Mark each random number with a prefix
    // according to its position in the send buffer
    for (i = 0; i < numProcs; i++)
        mySendBuffer[i] = myRandomNumber + i*100;
    myData = -1;

    if (myRank == inspectRank)
        printf("Rank %2.1i:\tmyData before the scatter is %i\n",
            inspectRank, myData);

    // Call the user's implementation of scatter
    scatter_int(mySendBuffer, 1, &myData, 1, 0, MPI_COMM_WORLD);
    if (myRank == inspectRank)
        printf("Rank %2.1i:\tmyData after the scatter is %i\n",
            inspectRank, myData);

    MPI_Scatter(mySendBuffer, 1, MPI_INT, &myData, 1, MPI_INT, 0,
        MPI_COMM_WORLD);
    if (myRank == inspectRank)
        printf("Rank %2.1i:\tmyData after the MPI scatter is %i\n",
            inspectRank, myData);

    MPI_Barrier(MPI_COMM_WORLD);

    // ---- Gather operation ----

    if (myRank == inspectRank)
        printf("-------------------------------------\n"
               "gather\n"
               "-------------------------------------\n");

    // Prepare the local data
    for (i = 0; i < numProcs; i++)
        myRecvBuffer[i] = -1;
    myData = myRandomNumber;

    if (myRank == inspectRank)
    {
        printf("Rank %2.1i:\tmyRecvBuffer before the gather is",
            inspectRank);
        for (i = 0; i < numProcs; i++)
            printf(" %i", myRecvBuffer[i]);
        printf("\n");
    }

    // Call the user's implementation of gather
    gather_int(&myData, 1, myRecvBuffer, 1, 0, MPI_COMM_WORLD);
    if (myRank == inspectRank)
    {
        printf("Rank %2.1i:\tmyRecvBuffer after the gather is",
            inspectRank);
        for (i = 0; i < numProcs; i++)
            printf(" %i", myRecvBuffer[i]);
        printf("\n");
    }

    MPI_Gather(&myData, 1, MPI_INT, myRecvBuffer, 1, MPI_INT, 0,
        MPI_COMM_WORLD);
    if (myRank == inspectRank)
    {
        printf("Rank %2.1i:\tmyRecvBuffer after the MPI gather is",
            inspectRank);
        for (i = 0; i < numProcs; i++)
            printf(" %i", myRecvBuffer[i]);
        printf("\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // ---- All-to-all operation ----

    if (myRank == inspectRank)
        printf("-------------------------------------\n"
               "all-to-all\n"
               "-------------------------------------\n");

    // Prepare the local data
    for (i = 0; i < numProcs; i++)
    {
        mySendBuffer[i] = myRandomNumber + 100*i;
        myRecvBuffer[i] = -1;
    }

    if (myRank == inspectRank)
    {
        printf("Rank %2.1i:\tmyRecvBuffer before the all-to-all is",
            inspectRank);
        for (i = 0; i < numProcs; i++)
            printf(" %i", myRecvBuffer[i]);
        printf("\n");
    }

    // Call the user's implementation of all-to-all
    alltoall_int(mySendBuffer, 1, myRecvBuffer, 1, MPI_COMM_WORLD);
    if (myRank == inspectRank)
    {
        printf("Rank %2.1i:\tmyRecvBuffer ater the all-to-all is",
            inspectRank);
        for (i = 0; i < numProcs; i++)
            printf(" %i", myRecvBuffer[i]);
        printf("\n");
    }

    MPI_Alltoall(mySendBuffer, 1, MPI_INT, myRecvBuffer, 1, MPI_INT,
         MPI_COMM_WORLD);
    if (myRank == inspectRank)
    {
        printf("Rank %2.1i:\tmyRecvBuffer ater the MPI all-to-all is",
            inspectRank);
        for (i = 0; i < numProcs; i++)
            printf(" %i", myRecvBuffer[i]);
        printf("\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // ---- Global reduction operation ----

    if (myRank == inspectRank)
        printf("-------------------------------------\n"
               "reduce\n"
               "-------------------------------------\n");

    // Prepare the local data
    myData = -1;

    if (myRank == inspectRank)
        printf("Rank %2.1i:\tmyData before the reduction is %i\n",
            inspectRank, myData);

    // Call the user's implementation of global reduction to sum all ranks
    sum_int(&myRank, &myData, 0, MPI_COMM_WORLD);
    if (myRank == inspectRank)
        printf("Rank %2.1i:\tmyData after the reduction is %i\n",
            inspectRank, myData);

    MPI_Reduce(&myRank, &myData, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (myRank == inspectRank)
        printf("Rank %2.1i:\tmyData after the MPI reduction is %i\n",
            inspectRank, myData);

    MPI_Finalize();

    return 0;
}
