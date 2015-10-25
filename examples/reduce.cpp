#include "utils/bind.hpp"
#include "utils/reduce.hpp"

int main(){
    const int N = 100;
    std::vector<bind::ptr<int> > a(N, bind::ptr<int>(1));

    bind::reduce(a, [](bind::ptr<int>& a, const bind::ptr<int>& b){
        bind::cpu([](bind::ptr<int>& dst, const bind::ptr<int>& src){
            *dst += *src;
        }, a, b);
    });

    std::cout << "Reduced value: " << a[0] << "\n";
    return 0;
}
