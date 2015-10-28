#include "utils/bind.hpp"
#include "utils/reduce.hpp"

int main(){
    const int N = 100;
    std::vector<bind::ptr<int> > a = {1, 2, 3, 4, 6, 7, 8, 9};

    bind::reduce(a, [](bind::ptr<int>& a, const bind::ptr<int>& b){
        bind::cpu([](bind::ptr<int>& dst, const bind::ptr<int>& src){
            *dst += *src;
        }, a, b);
    });

    bind::sync();
    std::cout << "Reduced value: " << a[0] << "\n";
    return 0;
}
