#include "utils/testing.hpp"

TEST_CASE( "Matrix difference is computed", "[sub]" )
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
 
    C_ = A_ - B_; 
    C  = A  - B; 

    REQUIRE((C == C_));

    A_ -= B_; 
    A  -= B; 

    REQUIRE((A == A_));
}
