SHELL = /bin/bash
PROG = integration
SRC = $(PROG).c
EXE = $(PROG).exe
NPROCS = 4

all: clean release run

debug:
	$(MPICC) $(FLAGS_DEBUG) $(SRC) -o $(EXE) -lm

release:
	$(MPICC) $(FLAGS_FAST) $(SRC) -o $(EXE) -lm

run:
	@echo "Usage: make run [NPROCS=<#procs> | N=<#intervals> | RANGE=<#steps>]"
	${MPIEXEC} -np $(NPROCS) $(EXE) $(N) $(RANGE)

analyze: release
	rm -rf test.1.er
	module load studio; ${MPIEXEC} -np $(NPROCS) collect -m on $(EXE); analyzer test.1.er;

totalview: debug
	module load totalview; ${MPIEXEC} -np $(NPROCS) -tv $(EXE)

scorep_profile:
	module load UNITE scorep; MPIEXEC="$(MPIEXEC) -x LD_LIBRARY_PATH -x SCOREP_EXPERIMENT_DIRECTORY=scorep -x SCOREP_ENABLE_PROFILING=1 -x SCOREP_ENABLE_TRACING=0" MPICC="scorep $(MPICC)" gmake clean debug run

scorep_trace:
	module load UNITE scorep; MPIEXEC="$(MPIEXEC) -x LD_LIBRARY_PATH -x SCOREP_EXPERIMENT_DIRECTORY=scorep -x SCOREP_ENABLE_PROFILING=0 -x SCOREP_ENABLE_TRACING=1" MPICC="scorep $(MPICC)" gmake clean debug run

vampir: scorep_trace
	module load UNITE vampir; vampir scorep/traces.otf2

cube: scorep_profile
	module load UNITE cube/4.3.3-intel; cube $(PWD)/scorep/profile.cubex

clean:
	@rm -f $(EXE) core* *.o *.mod a.out
	@rm -rf scorep
