Bind
=======

### Description
Bind is a parallel programming model for C++ applications that allows to quickly parallelise sequential codes without heavy refactoring. Resulting performance can be easily on pair with hand-tuned MPI-based applications and orders of magnitude faster than Apache Spark or Hadoop based implementations.  
See the [array.cpp](examples/array.cpp) example for a quick start.  
For overview of the model concept check the [white paper](http://arxiv.org/abs/1606.04830).

### Usage
Make sure that C++11 and Cilk Plus is enabled (i.e. -std=c++11 -fcilkplus) and include bind.hpp from your source code.  
Including mpi.h or cuda_runtime.h before bind.hpp will automatically enable respective functionality given that the compiler supports MPI and/or CUDA.  
The current implementation was tested with gcc/4.9 and icc/15 (with cuda/7.5 and mvapich2/impi/cray for MPI support).

### License
    Distributed under the Boost Software License, Version 1.0.  
    (See http://www.boost.org/LICENSE_1_0.txt)
