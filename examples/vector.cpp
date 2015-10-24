#include "bind.hpp"

template<typename T>
void reverse(bind::vector<T>& vec){
    int start = 0;
    int end = vec.size();
    while((start != end) && (start != --end)){
        std::swap(vec[start],vec[end]); start++;
    }
}

int main(){
    bind::vector<int> a(100);         // zero initialised vector
    bind::cpu(reverse<int>, a);  // reverse vector asynchronously
    bind::cpu([](bind::vector<int>& vec){ reverse(vec); }, a);
    bind::sync();                     // wait for operations to finish

    //std::vector<int, bind::default_allocator> test(12);
    return 0;
}
