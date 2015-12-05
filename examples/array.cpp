#include "utils/bind.hpp"

template<typename T>
using array = bind::array<T>;

template<typename T>
void reverse(array<T>& arr){
    int start = 0;
    int end = arr.size();
    while((start != end) && (start != --end)){
        std::swap(arr[start],arr[end]); start++;
    }
}

int main(){
    array<int> a(100);
    {
        bind::node last(bind::nodes::begin()+2);
        bind::cpu([](array<int>::iterator first, array<int>::iterator last){
            while(first != last) *first++ = last-first;
        }, a.begin(), a.end());

        bind::cpu(std::sort<array<int>::iterator>, a.begin(), a.end());
    }
        bind::node(bind::nodes::begin()+1).cpu(reverse<int>, a);
    {
        bind::node first(bind::nodes::begin());
        bind::cpu([](array<int>::const_iterator& first, array<int>::const_iterator& last){
            while(first != last) std::cout << *first++ << " ";
        }, a.cbegin(), a.cend());
    }
    bind::sync();
    return 0;
}
