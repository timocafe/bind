#include "utils/testing.hpp"

TEST_CASE( "Matrix is constructed and resized", "[resize]" )
{
    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);

    A_.resize(TEST_M,TEST_M/2);
    A.resize(TEST_M,TEST_M/2);

    REQUIRE((A == A_));
}
