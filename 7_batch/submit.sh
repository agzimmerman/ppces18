#!/usr/bin/env zsh

# Job name
#BSUB -J myBatchJob
# Limit run time to 5 minutes
#BSUB -W 00:05
# Request 1000 MB RAM per slot
#BSUB -M 1000
# Specify custom locations for the standard output and error
#BSUB -o stdout.log
#BSUB -e stderr.log
# Request 4 slots (= MPI processes)
#BSUB -n 4
#BSUB -a openmpi

$MPIEXEC $FLAGS_MPI_BATCH jacobi.exe < input
