#include "utils/testing.hpp"

TEST_CASE( "Matrix LQ factorization is computed", "[lq]" )
{
    matrix<double> A(TEST_M,TEST_N);
    matrix<double> L(TEST_M,TEST_N);
    matrix<double> Q(TEST_M,TEST_N);

    matrix_<double> A_(TEST_M,TEST_N);
    matrix_<double> L_(TEST_M,TEST_N);
    matrix_<double> Q_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);
 
    lq(A,  L,  Q);
    lq(A_, L_, Q_);

    REQUIRE((L_ == L));
    REQUIRE((Q_ == Q));
}
