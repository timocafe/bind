#include "utils/bind.hpp"
#define N 9

void norm(bind::ptr<double>& norm, bind::vector<double>& vec){
    for(int i = 0; i < vec.size(); i++) *norm += vec[i]*vec[i];
    *norm = std::sqrt(*norm);
}

int main(){
    bind::ptr<double> n(0);
    bind::vector<double> vec(N, 1);

    bind::cpu(norm, n, vec);
    bind::sync();

    std::cout << "Norm: " << n << "\n";
    return 0;
}
