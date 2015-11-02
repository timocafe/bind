# Makefile example (icc)

#FLAGS?= -std=c++11 -O3
FLAGS?= -std=c++11 -O3 -mt_mpi
EXAMPLES = array ptr reduce diffusion filter

.DEFAULT: all
.PHONY: all clean
all: $(EXAMPLES)

%: %.cpp
	$(CXX) -o $(basename $<) $< $(FLAGS)

clean:
	-rm -f $(EXAMPLES)
