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
#include "eckit/linalg/LinearAlgebraSparse.h"
#include "util.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

/// Test sparse linear algebra interface

CASE("test backend") {
    using linalg::Matrix;
    using linalg::SparseMatrix;
    using linalg::Vector;

    SparseMatrix Q(2, 4, {{0, 0, 2.}, {1, 2, 3.}});
    auto A = S(3, 3, 4, 0, 0, 2., 0, 2, -3., 1, 1, 2., 2, 2, 2.);
    auto x = V(3, 1., 2., 3.);

    const auto& linalg = linalg::LinearAlgebraSparse::backend();
    Log::info() << linalg << std::endl;

    SECTION("spmv - sparse 2x4 x vector 4 = vector 2") {
        auto b = V(4, 5., 5., 5., 5.);
        auto y = V(2, -42., -42.);
        auto z = V(2, 10., 15.);

        linalg.spmv(Q, b, y);
        EXPECT(equal_dense_matrix(y, z));
        EXPECT_THROWS_AS(linalg.spmv(Q, Vector(3), y), AssertionFailed);
    }

    SECTION("spmv - sparse identity 3x3 x vector 3 = vector 3") {
        Vector y(3);

        SparseMatrix B;
        B.setIdentity(3, 3);

        linalg.spmv(B, x, y);
        EXPECT(equal_dense_matrix(y, x));
    }

    SECTION("spmv - sparse identity 6x3 x vector 3 = vector 6") {
        SparseMatrix C;
        C.setIdentity(6, 3);

        Vector y(6);
        linalg.spmv(C, x, y);

        EXPECT(equal_dense_matrix(y, x, 3));
        EXPECT(equal_array(y.data() + 3, V(3, 0., 0., 0.).data(), 3));
    }

    SECTION("spmv - sparse identity 2x3 x vector 3 = vector 2") {
        SparseMatrix D;
        D.setIdentity(2, 3);

        Vector y(2);
        linalg.spmv(D, x, y);

        EXPECT(equal_dense_matrix(y, x, 2));
    }

    SECTION("spmv - sparse 3x3 x vector 3 = vector 3") {
        Vector y(3);
        linalg.spmv(A, x, y);

        EXPECT(equal_dense_matrix(y, V(3, -7., 4., 6.)));

        Log::info() << "spmv of sparse matrix and vector of non-matching sizes should fail" << std::endl;
        EXPECT_THROWS_AS(linalg.spmv(A, Vector(2), y), AssertionFailed);
    }

    SECTION("spmm - sparse 2x4 x matrix 4x2 = matrix 2x2") {
        auto B = M(4, 2, 5., 7., 5., 7., 5., 7., 5., 7.);
        auto Y = M(2, 2, -42., -42., -42., -42.);
        auto Z = M(2, 2, 10., 14., 15., 21.);

        linalg.spmm(Q, B, Y);
        EXPECT(equal_dense_matrix(Y, Z));
        EXPECT_THROWS_AS(linalg.spmm(Q, Matrix(2, 2), Y), AssertionFailed);
    }

    SECTION("spmm - sparse 3x3 x matrix 3x2 = matrix 3x2") {
        Matrix C(3, 2);
        linalg.spmm(A, M(3, 2, 1., 2., 3., 4., 5., 6.), C);

        EXPECT(equal_dense_matrix(C, M(3, 2, -13., -14., 6., 8., 10., 12.)));

        Log::info() << "spmm of sparse matrix and matrix of non-matching sizes should fail" << std::endl;
        EXPECT_THROWS_AS(linalg.spmm(A, Matrix(2, 2), C), AssertionFailed);
    }

    SECTION("dsptd - diagonal 3 x sparse 3x3 x diagonal 3 = sparse 3x3") {
        SparseMatrix B;
        linalg.dsptd(x, A, x, B);

        linalg::Index outer[4] = {0, 2, 3, 4};
        linalg::Index inner[4] = {0, 2, 1, 2};
        linalg::Scalar data[4] = {2., -9., 8., 18.};
        EXPECT(equal_sparse_matrix(B, outer, inner, data));

        Log::info() << "dsptd with matrices of non-matching sizes should fail" << std::endl;
        EXPECT_THROWS_AS(linalg.dsptd(x, A, Vector(2), B), AssertionFailed);
    }

    SECTION("dsptd - diagonal 2 x sparse 2x3 x diagonal 3 = sparse 2x3") {
        auto x = V(2, 1., 2.);
        auto y = V(3, 1., 2., 3.);
        auto Q = S(2, 3, 4, 0, 0, 1., 0, 2, 2., 1, 0, 3., 1, 1, 4.);

        SparseMatrix B;
        linalg.dsptd(x, Q, y, B);
        // B.save("B.mat");

        linalg::Index outer[4] = {0, 2, 4};
        linalg::Index inner[4] = {0, 2, 0, 1};
        linalg::Scalar data[4] = {1., 6., 6., 16.};
        EXPECT(equal_sparse_matrix(B, outer, inner, data));

        Log::info() << "dsptd with matrices of non-matching sizes should fail" << std::endl;
        EXPECT_THROWS_AS(linalg.dsptd(y, Q, x, B), AssertionFailed);
    }

    SECTION("dsptd - diagonal 2 x sparse 2x4 x diagonal 4 = sparse 2x4") {
        auto x = V(2, 1., 2.);
        auto y = V(4, 3., 4., 5., 6.);

        SparseMatrix B;
        linalg.dsptd(x, Q, y, B);

        linalg::Index outer[4] = {0, 1, 2};
        linalg::Index inner[4] = {0, 2};
        linalg::Scalar data[4] = {6., 30.};
        EXPECT(equal_sparse_matrix(B, outer, inner, data));

        Log::info() << "dsptd with matrices of non-matching sizes should fail" << std::endl;
        EXPECT_THROWS_AS(linalg.dsptd(y, Q, x, B), AssertionFailed);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);

    // Set sparse linear algebra backend
    eckit::linalg::LinearAlgebraSparse::backend(eckit::Resource<std::string>("-linearAlgebraSparseBackend", "generic"));

    return eckit::testing::run_tests(argc, argv, false);
}
