#include "utils/bind.hpp"

int main(){
    bind::ptr<double> sum;
    int a = 10, b = 20;

    {
        bind::node first = bind::nodes::begin();
        bind::cpu([](bind::ptr<double>& sum_, int a, int b){
            *sum_ = a + b;
        }, sum, a, b);
    }

    std::cout << sum << "\n";
    return 0;
}
