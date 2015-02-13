#include "utils/testing.hpp"

TEST_CASE( "Square root of diagonal matrix is computed", "[sqrt]" )
{
    diagonal<double>  A(TEST_M, TEST_M);
    diagonal_<double> A_((size_t)TEST_M);

    generate(A);
    A_ = cast<diagonal_<double> >(A);

    A_ = sqrt(A_);
    sqrt_inplace(A);

    REQUIRE((A == A_));
}
