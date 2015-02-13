# Makefile example (icc)

SHELL := /bin/bash
CXXFLAGS?= -std=c++11 -O3 -DNDEBUG -DAMBIENT_DISABLE_MPI

.DEFAULT: all
.PHONY: all clean

EXAMPLES = diffusion partitioned_vector vector atomic matrix mp_gemm_simple mp_gemm_log groups

all: libambient.a $(EXAMPLES)

%: %.cpp libambient.a
	$(CXX) -o $(basename $<) $< $(CXXFLAGS) -I../src -L. -lambient -mkl

libambient.a: ../src/ambient/ambient.cpp
	$(CXX) -c ../src/ambient/ambient.cpp $(CXXFLAGS) -I../src -o ambient.cpp.o
	ar cr libambient.a ambient.cpp.o; ranlib libambient.a;
	rm -f ambient.cpp.o

clean:
	-rm -f libambient.a $(EXAMPLES)
