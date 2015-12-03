#include <mpi.h>
#include "utils/bind.hpp"

int main(){
    bind::shared_ptr<int> max_rank = 0;

    std::for_each(bind::nodes::begin(), bind::nodes::end(), [&](int i){
        bind::node n(i);
        bind::cpu([](bind::shared_ptr<int>& max){
            *max = std::max(*max, bind::rank());
        }, max_rank);
    });

    bind::sync();
    std::cout << max_rank << std::endl;

    return 0;
}
