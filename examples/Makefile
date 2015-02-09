# Makefile
SHELL := /bin/bash
AMBIENT_INCLUDE?= -I../src
AMBIENT_LIB_PATH?=.
CXXFLAGS?= -std=c++11 -O3 -DNDEBUG -DAMBIENT_DISABLE_MPI

#CXXFLAGS_GCC?= -fopenmp
#CXXFLAGS_CLANG?= -Wno-array-bounds -DAMBIENT_SERIAL
#CXXFLAGS_ICC_OSX?= -Wfatal-errors -gcc-name=/opt/local/bin/gcc-mp-4.7 -gxx-name=/opt/local/bin/g++-mp-4.7 -gcc-version=473 -I/opt/local/include/gcc47/c++ -I/opt/local/include/gcc47/c++/x86_64-apple-darwin13/ -L/opt/local/lib/gcc47/

.DEFAULT: all
.PHONY: all clean

all: libambient.a diffusion partitioned_vector vector atomic matrix mp_gemm_simple mp_gemm_log groups

diffusion: libambient.a diffusion.cpp
	$(CXX) diffusion.cpp -o diffusion $(CXXFLAGS) -lambient -L$(AMBIENT_LIB_PATH) -I$(AMBIENT_INCLUDE)

partitioned_vector: libambient.a partitioned_vector.cpp
	$(CXX) partitioned_vector.cpp -o partitioned_vector $(CXXFLAGS) -lambient -L$(AMBIENT_LIB_PATH) -I$(AMBIENT_INCLUDE)

vector: libambient.a vector.cpp
	$(CXX) vector.cpp -o vector $(CXXFLAGS) -lambient -L$(AMBIENT_LIB_PATH) -I$(AMBIENT_INCLUDE)

atomic: libambient.a atomic.cpp
	$(CXX) atomic.cpp -o atomic $(CXXFLAGS) -lambient -L$(AMBIENT_LIB_PATH) -I$(AMBIENT_INCLUDE)

matrix: libambient.a matrix.cpp
	$(CXX) matrix.cpp -o matrix $(CXXFLAGS) -lambient -L$(AMBIENT_LIB_PATH) -I$(AMBIENT_INCLUDE) -mkl

mp_gemm_simple: libambient.a mp_gemm_simple.cpp
	$(CXX) mp_gemm_simple.cpp -o mp_gemm_simple $(CXXFLAGS) -lambient -L$(AMBIENT_LIB_PATH) -I$(AMBIENT_INCLUDE) -mkl

mp_gemm_log: libambient.a mp_gemm_log.cpp
	$(CXX) mp_gemm_log.cpp -o mp_gemm_log $(CXXFLAGS) -lambient -L$(AMBIENT_LIB_PATH) -I$(AMBIENT_INCLUDE) -mkl

groups: libambient.a groups.cpp
	$(CXX) groups.cpp -o groups $(CXXFLAGS) -lambient -L$(AMBIENT_LIB_PATH) -I$(AMBIENT_INCLUDE) -mkl

libambient.a: $(AMBIENT_INCLUDE)/ambient/ambient.cpp
	$(CXX) -c $(AMBIENT_INCLUDE)/ambient/ambient.cpp $(CXXFLAGS) -I$(AMBIENT_INCLUDE) -o ambient.cpp.o
	ar cr libambient.a ambient.cpp.o; ranlib libambient.a;
	rm -f ambient.cpp.o

clean:
	-rm -f libambient.a
	-rm -f diffusion
	-rm -f partitioned_vector
	-rm -f vector
	-rm -f atomic
	-rm -f matrix
	-rm -f mp_gemm_simple
	-rm -f mp_gemm_log
	-rm -f groups
