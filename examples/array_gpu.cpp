#ifdef CUDA_C
#include <cuda_runtime.h>

__global__ void inc_kernel(int* x){
    x[threadIdx.x] += threadIdx.x+2;
}

void launch_inc_kernel(int* x){
    inc_kernel <<< 1, 16 >>> ( x ); 
}

#else
#include <mpi.h>
#include <cuda_runtime.h>
#include "utils/bind.hpp"
#define N 100

void launch_inc_kernel(int* x);

template<typename T>
using array = bind::array<T>;

int main(){
    array<int> a(N);
    std::cout << "Parallel: " << bind::num_procs() << " procs x "
                              << bind::num_threads() << " threads\n";

    bind::cpu([](array<int>::iterator first, array<int>::iterator last){
        while(first != last) *first++ = last-first;
    }, a.begin(), a.end());

    bind::cpu(std::sort<array<int>::iterator>, a.begin(), a.end());

    {   bind::node first(0);

        bind::gpu([](array<int>::iterator first, array<int>::iterator last){
            launch_inc_kernel(&*first);
        }, a.begin(), a.end());
    }

    {   bind::node second(1);

        bind::gpu([](array<int>::iterator first, array<int>::iterator last){
            launch_inc_kernel(&*first);
        }, a.begin(), a.end());
    }

    bind::sync();
    return 0;
}

#endif
