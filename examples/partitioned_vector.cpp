#include "ambient/ambient.hpp"
#include "ambient/container/vector.hpp"
#include "ambient/container/partitioned_vector.hpp"

int main(){

    ambient::partitioned_vector<ambient::vector<int>, 3> a(10, 13);
    ambient::partitioned_vector<ambient::vector<int>, 4> b(10, 39);
    ambient::partitioned_vector<ambient::vector<int>, 5> c(10, 43);

    ambient::for_each(a.begin(), a.end(), [](int& e){ e++; });
    ambient::for_each(a.begin()+1, a.end()-1, [](int& e){ e++; });
    ambient::fill(a.begin(), a.end(), 169);
    ambient::generate(a.begin(), a.end(), []{ return 159; }); 
    ambient::sequence(a.begin(), a.end());
    ambient::transform(a.begin()+2, a.end(), b.begin()+2, c.begin()+2, std::plus<int>());
    ambient::copy(b.begin(), b.begin()+2, c.begin());
    ambient::transform(c.begin(), c.end(), b.begin(), [](int i){ return ++i; });
    ambient::replace(b.begin(), b.end(), 40, 87);

    for(int i = 0; i < 10; i++) ambient::cout << "Value is " << b[i] << "\n";

    ambient::sync();
    return 0;
}
