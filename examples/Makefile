# Makefile example (icc)

SHELL := /bin/bash
CXXFLAGS?= -std=c++11 -O3 -mt_mpi -I../src -ldl
#CXXFLAGS?= -std=c++11 -O3 -DBIND_MPI=MPI_DISABLE -I../src -ldl

.DEFAULT: all
.PHONY: all clean

EXAMPLES = diffusion vector reduce ptr
all: $(EXAMPLES)

%: %.cpp
	$(CXX) -o $(basename $<) $< $(CXXFLAGS)

clean:
	-rm -f $(EXAMPLES)
