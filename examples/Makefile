# Makefile
# Note: MPI can be easily disabled (i.e. removing BIND_REQUIRE_MPI knob and setting CXX to g++/icpc).

CXX = mpicxx
FLAGS = -O3 -std=c++11 -fcilkplus -DBIND_REQUIRE_MPI
EXAMPLES = diffusion.node mapreduce.node max_rank.node reduce.node shared_ptr.node

.DEFAULT: all
.PHONY: all clean
all: $(EXAMPLES)

%.node: %.cpp
	$(CXX) -o $@ $< $(FLAGS)

run:
	# Note: MV2_ENABLE_AFFINITY=0 enables threading in MVAPICH2 
	for example in $(EXAMPLES); do \
		MV2_ENABLE_AFFINITY=0 mpiexec -np 2 ./$$example || exit 1; \
	done

clean:
	-rm -f $(EXAMPLES)
