SHELL = /bin/bash
#name of input file
INPUT=input
PROG = jacobi
SRC = $(PROG).c
EXE = $(PROG).exe
NPROCS = 4

all: clean release run

debug:
	$(MPICC) $(FLAGS_DEBUG) $(SRC) -o $(EXE) -lm

release:
	$(MPICC) $(FLAGS_FAST) $(SRC) -o $(EXE) -lm

runParallel:
	@echo "Usage: make run [NPROCS=<#procs>] [INPUT=<name of input file>]"
	${MPIEXEC} -np $(NPROCS) $(EXE) < $(INPUT)

run:
	@echo "Usage: make run [INPUT=<name of input file>]"
	$(EXE) < $(INPUT)

analyze: release
	rm -rf test.1.er
	module load studio; ${MPIEXEC} -np $(NPROCS) < $(INPUT) collect -m on $(EXE); analyzer test.1.er

totalview: debug
	module load totalview; ${MPIEXEC} -np $(NPROCS) -tv $(EXE) < $(INPUT)

scorep_profile:
	module load UNITE scorep; MPIEXEC="$(MPIEXEC) -x LD_LIBRARY_PATH -x SCOREP_EXPERIMENT_DIRECTORY=scorep -x SCOREP_ENABLE_PROFILING=1 -x SCOREP_ENABLE_TRACING=0" MPICC="scorep $(MPICC)" gmake clean debug runParallel

scorep_trace:
	module load UNITE scorep; MPIEXEC="$(MPIEXEC) -x LD_LIBRARY_PATH -x SCOREP_EXPERIMENT_DIRECTORY=scorep -x SCOREP_ENABLE_PROFILING=0 -x SCOREP_ENABLE_TRACING=1" MPICC="scorep $(MPICC)" gmake clean debug runParallel

vampir: scorep_trace
	module load UNITE vampir; vampir scorep/traces.otf2

cube: scorep_profile
	module load UNITE cube/4.3.3-intel; cube $(PWD)/scorep/profile.cubex

clean:
	@rm -f $(EXE) core* *.o *.mod a.out
	@rm -rf scorep
