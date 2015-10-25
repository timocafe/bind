# Makefile example (icc)

#FLAGS?= -std=c++11 -O3
FLAGS?= -std=c++11 -O3 -DBIND_MPI=MPI_THREAD_FUNNELED -mt_mpi
EXAMPLES = diffusion vector reduce ptr

.DEFAULT: all
.PHONY: all clean
all: $(EXAMPLES)

%: %.cpp
	$(CXX) -o $(basename $<) $< $(FLAGS)

clean:
	-rm -f $(EXAMPLES)
