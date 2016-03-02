#ifdef CUDA_C
#include <cuda_runtime.h>

__global__ void inc_kernel(int* x){
    x[threadIdx.x] += threadIdx.x;
}

void launch_inc_kernel(int* x, int length){
    inc_kernel <<< 1, length >>> ( x ); 
}

#else
#include "utils/bind.hpp"
template<typename T> using array = bind::array<T>;
void launch_inc_kernel(int* x, int length);

int main(){
    if(bind::num_procs() != 2){
        std::cout << "This example can use only two processes\n";
        return 0;
    }

    array<int> A(128);

    // Generating A on both nodes (no node declaration was present)
    bind::cpu(std::iota<array<int>::iterator, int>, A.begin(), A.end(), 0);

    // Printing A on the first node (no transfer occurs since A is common at the moment)
    bind::node(0).cpu([](array<int>::const_iterator& first, array<int>::const_iterator& last){
        while(first != last) std::cout << *first++ << " ";
        std::cout << "\n";
    }, A.cbegin(), A.cend());

    // Transfering A to the first node's GPU and incrementing values
    bind::node(0).gpu([](array<int>::iterator first, array<int>::iterator last){
        launch_inc_kernel(&*first, last-first);
    }, A.begin(), A.end());

    // Transfering A from the first node's GPU to the second node's GPU and incrementing it there
    bind::node(1).gpu([](array<int>::iterator first, array<int>::iterator last){
        launch_inc_kernel(&*first, last-first);
    }, A.begin(), A.end());

    // Transfering A back to first node's CPU and printing
    bind::node(0).cpu([](array<int>::const_iterator& first, array<int>::const_iterator& last){
        while(first != last) std::cout << *first++ << " ";
        std::cout << "\n";
    }, A.cbegin(), A.cend());

    // Waiting for the operations completion
    bind::sync();
    return 0;
}

#endif
