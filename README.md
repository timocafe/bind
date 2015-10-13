Bind
=======

### Installation
Bind is an expiremental shrink of Ambient header-only library (so just installing the sources is enough):  
```sh
cmake .  
make install
```

### Usage
Compilation of the target application against Bind's include folder with C++11 enabled is generally sufficient. Otherwise be sure to check the compilation options: threading backend and MPI mode are compiler-specific by default (so the respective compiler knobs might be needed). 

To enforce threading backend or MPI mode use the following knobs:

    -DBIND_THREADING [CILK, OPENMP or NONE]  
    -DBIND_MPI [MPI_DISABLE or the desired MPI threading level]

(Set BIND_VERBOSE environment variable to see the resulting configuration).

### License
    Distributed under the Boost Software License, Version 1.0.  
    (See http://www.boost.org/LICENSE_1_0.txt)
