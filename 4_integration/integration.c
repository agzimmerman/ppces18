#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define IDLETIME 0.1

#define TAG_WORK 0
#define TAG_END 2

double func(double x)
{
    double t = MPI_Wtime();

    // Introduce work imballance by sleeping more given larger x
    while (MPI_Wtime()-t <= IDLETIME*x*x);
    return 4.0 / (1.0 + x*x);
}

double integrate(double (*f)(double x),
                 double x_start,
                 double x_end,
                 int maxSteps)
{
    int myRank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    double sum = 0.0;
    double x[2], y[2];

    double stepSize = (x_end - x_start)/(double)maxSteps;
    int step;
    int nextRank = 1;

    MPI_Status status;

    if (myRank == 0)
    {
        // I am the controller, distribute the work
        for (step = 0; step < maxSteps; step++)
        {
            x[0] = x_start + stepSize*step;
            x[1] = x_start + stepSize*(step+1);
            nextRank = step % (numProcs-1) + 1;
            // Send the work
            MPI_Send(x, 2, MPI_DOUBLE, nextRank, TAG_WORK, MPI_COMM_WORLD);
            // Receive the result
            MPI_Recv(y, 2, MPI_DOUBLE, nextRank, TAG_WORK, MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
            sum += stepSize*0.5*(y[0]+y[1]);
        }
        // Signal workers to stop by sending empty messages with tag TAG_END
        for (nextRank = 1; nextRank < numProcs; nextRank++)
            MPI_Send(&nextRank, 0, MPI_INT, nextRank, TAG_END, MPI_COMM_WORLD);
    }
    else
    {
        while (1)
        {
            // I am a worker, wait for work

            // Receive the left and right points of the trapezoid and compute
            // the corresponding function values. If the tag is TAG_END, don't
            // compute but exit.
            MPI_Recv(x, 2, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
                &status);
            if (status.MPI_TAG == TAG_END) break;
            y[0] = f(x[0]);
            y[1] = f(x[1]);
            // Send back the computed result
            MPI_Send(y, 2, MPI_DOUBLE, 0, TAG_WORK, MPI_COMM_WORLD);
        }
    }
    return sum;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int myRank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    // Integration domain is [0, 1]
    double x0 = 0.0, x1 = 1.0;
    int maxSteps = 100;

    if (myRank == 0)
    {
        if (argc > 1)
        {
            maxSteps = atoi(argv[1]);
            if (maxSteps < 1) MPI_Abort(MPI_COMM_WORLD, 1);
        }
        printf("Integrating from %lf to %lf in %i steps\n",
            x0, x1, maxSteps);
    }

    // Synchronize before making performance measurements
    MPI_Barrier(MPI_COMM_WORLD);

    double startTime = MPI_Wtime();

    double pi = integrate(func, x0, x1, maxSteps);

    double stopTime = MPI_Wtime();

    if (myRank == 0)
        printf("\nPI = %lf\tintegral = %lf\nComputation took %.3lf seconds\n",
            M_PI, pi, stopTime-startTime);

    MPI_Finalize();
    return 0;
}
