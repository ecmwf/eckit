/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/linalg/LinearAlgebra.h"
#include "util.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

/// Test linear algebra interface

CASE("test backend") {
    using namespace linalg;

    Matrix A = M(2, 2, 1., -2., -4., 2.);
    SparseMatrix S(2, 4, {{0, 0, 2.}, {1, 2, 3.}});

    auto& linalg = LinearAlgebra::backend();
    Log::info() << linalg << std::endl;

    SECTION("dot") {
        Vector a = V(3, 1., 2., 4.);

        EXPECT(linalg.dot(a, a) == 21.);
        EXPECT_THROWS_AS(linalg.dot(a, Vector(2)), AssertionFailed);
    }

    SECTION("gemv") {
        Vector y = V(2, -42., -42.);
        Vector z = V(2, 3., 0.);

        linalg.gemv(A, V(2, -1., -2.), y);
        EXPECT(equal_dense_matrix(y, z));
        EXPECT_THROWS_AS(linalg.gemv(A, Vector(3), y), AssertionFailed);
    }

    SECTION("gemm - 2x2 x 2x2") {
        Matrix Y = M(2, 2, -42., -42., -42., -42.);
        Matrix Z = M(2, 2, 9., -6., -12., 12.);

        linalg.gemm(A, A, Y);
        EXPECT(equal_dense_matrix(Y, Z));
        EXPECT_THROWS_AS(linalg.gemm(A, Matrix(1, 2), Y), AssertionFailed);
    }

    SECTION("gemm - 2x4 x 4x3") {
        Matrix A       = M(2, 4, 1., 2., 3., 4., 5., 6., 7., 8.);
        Matrix B       = M(4, 3, 9., 10., 11., 12., 13., 14., 15., 16., 17., 18., 19., 20.);
        Matrix C       = M(2, 3, 0., 0., 0., 0., 0., 0.);
        Matrix C_check = M(2, 3,
                           150., 160., 170.,   // row=0
                           366., 392., 418.);  // row=1

        linalg.gemm(A, B, C);

        EXPECT(equal_dense_matrix(C, C_check));
    }

    SECTION("spmv") {
        Vector b = V(4, 5., 5., 5., 5.);
        Vector y = V(2, -42., -42.);
        Vector z = V(2, 10., 15.);

        linalg.spmv(S, b, y);
        EXPECT(equal_dense_matrix(y, z));
        EXPECT_THROWS_AS(linalg.spmv(S, Vector(3), y), AssertionFailed);
    }

    SECTION("spmm") {
        Matrix B = M(4, 2, 5., 7., 5., 7., 5., 7., 5., 7.);
        Matrix Y = M(2, 2, -42., -42., -42., -42.);
        Matrix Z = M(2, 2, 10., 14., 15., 21.);

        linalg.spmm(S, B, Y);
        EXPECT(equal_dense_matrix(Y, Z));
        EXPECT_THROWS_AS(linalg.spmm(S, Matrix(2, 2), Y), AssertionFailed);
    }

    SECTION("dsptd") {
        struct SparseMatrix : linalg::SparseMatrix {
            using linalg::SparseMatrix::SparseMatrix;
            Size size() const { return nonZeros(); }
        };

        Vector x = V(2, 1., 2.);
        Vector y = V(4, 3., 4., 5., 6.);
        SparseMatrix R(2, 4, {{0, 0, 6.}, {1, 2, 30.}});

        SparseMatrix P;
        linalg.dsptd(x, S, y, P);

        EXPECT(P.rows() == R.rows());
        EXPECT(P.cols() == R.cols());
        EXPECT(equal_sparse_matrix(P, R.outer(), R.inner(), R.data()));

        EXPECT_THROWS_AS( linalg.dsptd(y, S, x, P), AssertionFailed);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    using namespace eckit;

    Main::initialise(argc, argv);

    // Set linear algebra backend
    linalg::LinearAlgebra::backend(Resource<std::string>("-linearAlgebraBackend", "generic"));

    return testing::run_tests(argc, argv, false);
}
