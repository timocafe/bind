# Makefile for multithreaded examples.
# Note: Cilk is used as a threading backend (MPI and CUDA are not enabled).

CXX = g++
FLAGS = -O3 -std=c++11 -fcilkplus
EXAMPLES = array diffusion reduce shared_ptr

.DEFAULT: all
.PHONY: all clean
all: $(EXAMPLES)

%: %.cpp
	$(CXX) -o $(basename $<) $< $(FLAGS)

run:
	for example in $(EXAMPLES); do ./$$example || exit 1; done

clean:
	-rm -f $(EXAMPLES)
