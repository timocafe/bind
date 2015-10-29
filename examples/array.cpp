#include "utils/bind.hpp"

template<typename T>
using array = bind::array<T, bind::allocator>;

template<typename T>
void reverse(array<T>& vec){
    int start = 0;
    int end = vec.size();
    while((start != end) && (start != --end)){
        std::swap(vec[start],vec[end]); start++;
    }
}

int main(){

    array<int> a(100);
    bind::cpu([](array<int>& vec){
        for(int i = 0; i < vec.size(); i++) vec[i] = i;
    }, a);

    bind::cpu(reverse<int>, a);

    bind::cpu([](array<int>& vec){
        for(int i = 0; i < vec.size(); i++) std::cout << vec[i] << " ";
    }, a);


    bind::sync();

    return 0;
}
