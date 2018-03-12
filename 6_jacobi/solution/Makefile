SHELL = /bin/bash
#name of input file
INPUT = input
INPUT_BIG = $(INPUT).big
PROG = jacobi
SRC = $(PROG).c
EXE = $(PROG).exe
EXE_REP = $(PROG)_rep.exe
NPROCS = 4

all: clean release run

debug:
	$(MPICC) $(FLAGS_DEBUG) $(SRC) -o $(EXE) -lm

release:
	$(MPICC) $(FLAGS_FAST) $(SRC) -o $(EXE) -lm

reports:
	module load reports/6.0.6; $(MPICC) $(FLAGS_FAST) $(SRC) -o $(EXE_REP) -L/rwthfs/rz/SW/ddt/reports-6.0.6-RHEL7/lib/64 -lmap-sampler -Wl,--eh-frame-hdr -lm

runParallel:
	@echo "Usage: make run [NPROCS=<#procs>] [INPUT=<name of input file>]"
	${MPIEXEC} -np $(NPROCS) $(EXE) < $(INPUT)

run:
	@echo "Usage: make run [INPUT=<name of input file>]"
	$(EXE) < $(INPUT)

runReports: reports
	module load reports/6.0.6; perf-report $(MPI_ROOT)/bin/mpiexec -np $(NPROCS) $(EXE_REP) < $(INPUT_BIG)

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
	module load UNITE vampir; ls -l > /dev/null; vampir scorep/traces.otf2

cube: scorep_profile
	module load UNITE cube/4.3.3-intel; ls -l > /dev/null; cube $(PWD)/scorep/profile.cubex

clean:
	@rm -f $(EXE) $(EXE_REP) core* *.o *.mod a.out
	@rm -rf scorep
