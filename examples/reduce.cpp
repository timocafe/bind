#include "utils/bind.hpp"

int main(){
    std::vector<bind::shared_ptr<int> > a = {1, 2, 3, 4, 6, 7, 8, 9};

    for(int s = 1; s < a.size(); s *= 2)
    for(int i = s; i < a.size(); i += s*2){

        bind::node(i % bind::nodes::size()).cpu([](bind::shared_ptr<int>& dst, const bind::shared_ptr<int>& src){
            *dst += *src;
        }, a[i-s], a[i]);

    }

    bind::sync();
    std::cout << "Reduced value: " << a[0] << "\n";
    return 0;
}
