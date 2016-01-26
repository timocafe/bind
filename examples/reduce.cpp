#include "utils/bind.hpp"

int main(){
    std::vector<bind::shared_ptr<int> > a = {1, 2, 3, 4, 6, 7, 8, 9};

    for(int stride = 1; stride < a.size(); stride *= 2){
        for(int i = stride; i < a.size(); i += stride*2){

            bind::cpu([](bind::shared_ptr<int>& dst, const bind::shared_ptr<int>& src){
                *dst += *src;
            }, a[i-stride], a[i]);

        }
    }

    bind::sync();
    std::cout << "Reduced value: " << a[0] << "\n";
    return 0;
}
