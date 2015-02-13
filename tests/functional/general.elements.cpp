#include "utils/testing.hpp"

TEST_CASE( "Element access is performed", "[element_access]" )
{
    size_t accessx = TEST_M-1;
    size_t accessy = TEST_N-1;

    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A_, ambient::utils::Rd);
    A = cast<matrix<double> >(A_);

    REQUIRE((A(accessx,accessy) == A_(accessx,accessy)));
    REQUIRE((A == A_));

    A_(accessx,accessy) = 3;
    A (accessx,accessy) = 3;

    REQUIRE((A(accessx,accessy) == A_(accessx,accessy)));
    REQUIRE((A == A_));
}
