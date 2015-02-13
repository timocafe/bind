#include "utils/testing.hpp"

TEST_CASE( "Matrix trace is calculated", "[trace]" )
{
    matrix<double>  A (TEST_M,TEST_M);
    matrix_<double> A_(TEST_M,TEST_M);

    generate(A);
    A_ = cast<matrix_<double> >(A);

    double t_ = trace(A_);
    double t  = trace(A);

    REQUIRE_CLOSE(t, t_);
}
