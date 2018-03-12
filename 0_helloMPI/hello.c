#include <stdio.h>
#include <mpi.h>

int main (int argc, char* argv[])
{
    // Initialise the MPI library
    int ierr;

    ierr = MPI_Init(&argc, &argv);


    // Obtain the process ID and the number of processes
    int numberOfProcs;

    int rank;

    ierr = MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcs);
    
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    // Display the process ID and the number of processes
    printf("Hello world from rank %u of %u\n", rank, numberOfProcs);


    // Deinitialise the MPI library
    MPI_Finalize();


    return 0;
}
