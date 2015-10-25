#include "utils/bind.hpp"

int main(){
    bind::ptr<double> sum;
    int a = 10;
    int b = 20;

    bind::cpu([](bind::ptr<double>& sum_, int a, int b){
        *sum_ = a + b;
    }, sum, a, b);

    std::cout << "Pointed value: " << sum << "\n";
    return 0;
}
