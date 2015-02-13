#include "utils/testing.hpp"

TEST_CASE( "Matrix first column is removed", "[remove_first_col]" )
{
    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);

    remove_cols(A_,0,1);
    remove_cols(A,0,1);

    REQUIRE((A == A_));
}

TEST_CASE( "Matrix last column is removed", "[remove_last_col]" )
{
    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);

    remove_cols(A_, TEST_N-1, 1);
    remove_cols(A,  TEST_N-1, 1);

    REQUIRE((A == A_));
}

TEST_CASE( "Matrix column is removed", "[remove_one_col]" )
{
    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);
    int col =  ambient::utils::Rd.IntRd() % A_.num_cols();
    
    remove_cols(A_, col, 1);
    remove_cols(A,  col, 1);

    REQUIRE((A == A_));
}

TEST_CASE( "Some matrix columns are removed", "[remove_several_cols]" )
{
    matrix<double>  A (TEST_M,TEST_N);
    matrix_<double> A_(TEST_M,TEST_N);

    generate(A);
    A_ = cast<matrix_<double> >(A);
    int col =  ambient::utils::Rd.IntRd() % A_.num_cols();
    int numcols = TEST_N - col - 1;

    remove_cols(A_, col, numcols);
    remove_cols(A,  col, numcols);

    REQUIRE((A == A_));
}
