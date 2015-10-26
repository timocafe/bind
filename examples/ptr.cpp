#include "utils/bind.hpp"

int main(){
    bind::ptr<double> sum;
    bind::ptr< std::tuple<int, int, int> > test;


    int a = 10;
    int b = 20;
    int c = 30;

    {
        bind::node first = bind::nodes::begin();
        bind::cpu([](bind::ptr<double>& sum_, int a, int b){
            *sum_ = a + b;
        }, sum, a, b);

        bind::cpu([](bind::ptr< std::tuple<int,int,int> >& test_, int a, int b, int c){
            *test_ = std::make_tuple(a,b,c);
        }, test, a, b, c);
    }

    printf("Value : %.2f\n", sum.load());
    printf("Value : %d %d %d\n", std::get<0>(test.load()),  std::get<1>(test.load()), std::get<2>(test.load()));
    return 0;
}
