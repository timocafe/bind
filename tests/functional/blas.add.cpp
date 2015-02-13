#include "utils/testing.hpp"

TEST_CASE( "Matrix sum is computed", "[add]" )
{
    matrix<double> A(TEST_M,TEST_N);
    matrix<double> B(TEST_M,TEST_N);
    matrix<double> C(TEST_M,TEST_N);

    matrix_<double> A_(TEST_M,TEST_N);
    matrix_<double> B_(TEST_M,TEST_N);
    matrix_<double> C_(TEST_M,TEST_N);

    generate(A);
    generate(B);

    A_ = cast<matrix_<double> >(A);
    B_ = cast<matrix_<double> >(B);
    C_ = cast<matrix_<double> >(C);

    C  = A  + B; 
    C_ = A_ + B_;

    REQUIRE((C == C_));

    C  += B;
    C_ += B_;

    REQUIRE((C == C_));
}
