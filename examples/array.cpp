#include "utils/bind.hpp"
template<typename T> using array = bind::array<T>;

int main(){
    if(bind::num_procs() != 2){
        std::cout << "This example can use only two processes\n";
        return 0;
    }

    array<int> A(100);
    {
        // Marking where scope-subsequent operations will be executed.
        // Without node's declaration the ops are executed everywhere.
        bind::node last(1);
        // Generating and sorting A on the selected node
        // Note: the actual memory is allocated only upon the first touch
        bind::cpu(std::generate<array<int>::iterator, decltype(std::rand)>, A.begin(), A.end(), &std::rand);
        bind::cpu(std::sort<array<int>::iterator>, A.begin(), A.end());
    }

    // Reversing this array on the other node
    bind::node(0).cpu(std::reverse<array<int>::iterator>, A.begin(), A.end());
        
    // Printing the resulting array on the first node
    bind::node(0).cpu([](array<int>::const_iterator& first, array<int>::const_iterator& last){
        while(first != last) std::cout << *first++ << " "; // std::cout is enabled only on the first node
    }, A.cbegin(), A.cend());

    // Waiting for the operations completion
    bind::sync();
    return 0;
}
