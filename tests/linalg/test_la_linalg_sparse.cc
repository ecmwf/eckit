/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "./util.h"
#include "eckit/config/Resource.h"
#include "eckit/linalg/LinearAlgebra.h"
#include "eckit/linalg/SparseMatrix.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

/// Test sparse linear algebra interface

CASE("test backend") {
    using linalg::Matrix;
    using linalg::SparseMatrix;
    using linalg::Vector;

    SparseMatrix A(2, 4, {{0, 0, 2.}, {1, 2, 3.}});

    const auto& linalg = linalg::LinearAlgebraSparse::backend();
    Log::info() << linalg << std::endl;

    SECTION("spmv") {
        Vector b = V(4, 5., 5., 5., 5.);
        Vector y = V(2, -42., -42.);
        Vector z = V(2, 10., 15.);

        linalg.spmv(A, b, y);
        EXPECT(equal_dense_matrix(y, z));
        EXPECT_THROWS_AS(linalg.spmv(A, Vector(3), y), AssertionFailed);
    }

    SECTION("spmm") {
        Matrix B = M(4, 2, 5., 7., 5., 7., 5., 7., 5., 7.);
        Matrix Y = M(2, 2, -42., -42., -42., -42.);
        Matrix Z = M(2, 2, 10., 14., 15., 21.);

        linalg.spmm(A, B, Y);
        EXPECT(equal_dense_matrix(Y, Z));
        EXPECT_THROWS_AS(linalg.spmm(A, Matrix(2, 2), Y), AssertionFailed);
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
