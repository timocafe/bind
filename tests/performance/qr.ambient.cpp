#include "utils/testing.hpp"

TEST_CASE( "Matrix QR factorization performance measured", "[ambient::qr]" )
{
    measurement params;
    size_t x = params.num_cols();
    size_t y = params.num_rows();

    matrix<double> A(x, y);
    matrix<double> Q(x, y);
    matrix<double> R(x, y);

    matrix_<double> A_(x, y);
    matrix_<double> Q_(x, y);
    matrix_<double> R_(x, y);

    generate(A);
    A_ = cast<matrix_<double> >(A);
    ambient::sync();

    qr(A_, Q_, R_);
    qr(A,  Q,  R); 

    measurement::timer time("ambient"); time.begin();
    ambient::sync();
    time.end();

    params.report(gflops::gemm, time.get_time());

    REQUIRE((Q_ == Q));
    REQUIRE((R_ == R));
}

