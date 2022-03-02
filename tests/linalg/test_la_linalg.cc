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
    using linalg::LinearAlgebra;
    using linalg::Matrix;
    using linalg::SparseMatrix;
    using linalg::Vector;

    auto A = M(2, 2, 1., -2., -4., 2.);
    SparseMatrix S(2, 4, {{0, 0, 2.}, {1, 2, 3.}});

    Log::info() << LinearAlgebra::backend() << std::endl;

    SECTION("dot") {
        auto a = V(3, 1., 2., 4.);

        EXPECT(LinearAlgebra::dot(a, a) == 21.);
        EXPECT_THROWS_AS(LinearAlgebra::dot(a, Vector(2)), AssertionFailed);
    }

    SECTION("gemv") {
        auto y = V(2, -42., -42.);
        auto z = V(2, 3., 0.);

        LinearAlgebra::gemv(A, V(2, -1., -2.), y);
        EXPECT(equal_dense_matrix(y, z));
        EXPECT_THROWS_AS(LinearAlgebra::gemv(A, Vector(3), y), AssertionFailed);
    }

    SECTION("gemm") {
        auto Y = M(2, 2, -42., -42., -42., -42.);
        auto Z = M(2, 2, 9., -6., -12., 12.);

        LinearAlgebra::gemm(A, A, Y);
        EXPECT(equal_dense_matrix(Y, Z));
        EXPECT_THROWS_AS(LinearAlgebra::gemm(A, Matrix(1, 2), Y), AssertionFailed);
    }

    SECTION("spmv") {
        auto b = V(4, 5., 5., 5., 5.);
        auto y = V(2, -42., -42.);
        auto z = V(2, 10., 15.);

        LinearAlgebra::spmv(S, b, y);
        EXPECT(equal_dense_matrix(y, z));
        EXPECT_THROWS_AS(LinearAlgebra::spmv(S, Vector(3), y), AssertionFailed);
    }

    SECTION("spmm") {
        auto B = M(4, 2, 5., 7., 5., 7., 5., 7., 5., 7.);
        auto Y = M(2, 2, -42., -42., -42., -42.);
        auto Z = M(2, 2, 10., 14., 15., 21.);

        LinearAlgebra::spmm(S, B, Y);
        EXPECT(equal_dense_matrix(Y, Z));
        EXPECT_THROWS_AS(LinearAlgebra::spmm(S, Matrix(2, 2), Y), AssertionFailed);
    }

    SECTION("dsptd") {
        auto x = V(2, 1., 2.);
        auto y = V(4, 3., 4., 5., 6.);
        SparseMatrix R(2, 4, {{0, 0, 6.}, {1, 2, 30.}});

        SparseMatrix P;
        LinearAlgebra::dsptd(x, S, y, P);

        EXPECT(P.rows() == R.rows());
        EXPECT(P.cols() == R.cols());
        EXPECT(equal_sparse_matrix(P, R.outer(), R.inner(), R.data()));

        EXPECT_THROWS_AS(LinearAlgebra::dsptd(y, S, x, P), AssertionFailed);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    using eckit::linalg::LinearAlgebra;

    eckit::Main::initialise(argc, argv);

    // Set dense/sparse linear algebra backends
    eckit::Resource<std::string> backend("-linearAlgebraBackend", "generic");

    auto setDense  = LinearAlgebra::hasDenseBackend(backend);
    auto setSparse = LinearAlgebra::hasSparseBackend(backend);
    ASSERT(setDense || setSparse);

    if (setDense) {
        LinearAlgebra::denseBackend(backend);
    }

    if (setSparse) {
        LinearAlgebra::sparseBackend(backend);
    }

    return eckit::testing::run_tests(argc, argv, false);
}
