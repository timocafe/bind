# Makefile for multi-node examples.
# Note: Cilk is used as a threading backend, MPI for data transfers (CUDA is not enabled)

# A hint for MVAPICH2 users: run mpiexec with MV2_ENABLE_AFFINITY=0 to enable threading support.

CXX = mpicxx
FLAGS = -O3 -std=c++11 -fcilkplus -DBIND_REQUIRE_MPI
EXAMPLES = diffusion.node max_rank.node mapreduce.node shared_ptr.node

.DEFAULT: all
.PHONY: all clean
all: $(EXAMPLES)

%.node: %.cpp
	$(CXX) -o $@ $< $(FLAGS)

run:
	for example in $(EXAMPLES); do MV2_ENABLE_AFFINITY=0 mpiexec -np 2 ./$$example || exit 1; done

clean:
	-rm -f $(EXAMPLES)
