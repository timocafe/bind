#include "utils/testing.hpp"

TEST_CASE( "Matrix first rows are removed", "[remove_first_rows]" )
{
    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);

    remove_rows(A_, 0, 1);
    remove_rows(A,  0, 1);

    REQUIRE((A == A_));
}

TEST_CASE( "Matrix last rows are removed", "[remove_last_rows]" )
{
    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);

    remove_rows(A_, TEST_M-1, 1);
    remove_rows(A,  TEST_M-1, 1);

    REQUIRE((A == A_));
}

TEST_CASE( "Matrix row is removed", "[remove_row]" )
{
    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);

    int row = ambient::utils::Rd.IntRd() % A_.num_rows();
    remove_rows(A_, row, 1);
    remove_rows(A,  row, 1);

    REQUIRE((A == A_));
}

TEST_CASE( "Several matrix rows are removed", "[remove_several_rows]" )
{
    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);

    int row = ambient::utils::Rd.IntRd() % (TEST_M-1);
    int numrows = (int)(TEST_M-1 - row)/2;
    remove_rows(A_, row, numrows);
    remove_rows(A,  row, numrows);

    REQUIRE((A == A_));
}
