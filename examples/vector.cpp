#include "utils/bind.hpp"

template<typename T>
using vector = bind::vector<T, bind::allocator>;

template<typename T>
void reverse(vector<T>& vec){
    int start = 0;
    int end = vec.size();
    while((start != end) && (start != --end)){
        std::swap(vec[start],vec[end]); start++;
    }
}

int main(){

    vector<int> a(100);
    bind::cpu(reverse<int>, a);
    bind::cpu([](vector<int>& vec){ reverse(vec); }, a);
    bind::sync();

    return 0;
}
