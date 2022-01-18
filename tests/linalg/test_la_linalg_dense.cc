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
#include "eckit/linalg/LinearAlgebraDense.h"
#include "util.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

/// Test dense linear algebra interface

CASE("test backend") {
    using linalg::Matrix;
    using linalg::Vector;

    Matrix A = M(2, 2, 1., -2., -4., 2.);

    const auto& linalg = linalg::LinearAlgebraDense::backend();
    Log::info() << linalg << std::endl;

    SECTION("dot") {
        auto a = V(3, 1., 2., 4.);

        EXPECT(linalg.dot(a, a) == 21.);
        EXPECT_THROWS_AS(linalg.dot(a, Vector(2)), AssertionFailed);
    }

    SECTION("gemv") {
        auto y = V(2, -42., -42.);
        auto z = V(2, 3., 0.);

        linalg.gemv(A, V(2, -1., -2.), y);
        EXPECT(equal_dense_matrix(y, z));
        EXPECT_THROWS_AS(linalg.gemv(A, Vector(3), y), AssertionFailed);
    }

    SECTION("gemm - 2x2 x 2x2") {
        auto Y = M(2, 2, -42., -42., -42., -42.);
        auto Z = M(2, 2, 9., -6., -12., 12.);

        linalg.gemm(A, A, Y);
        EXPECT(equal_dense_matrix(Y, Z));
        EXPECT_THROWS_AS(linalg.gemm(A, Matrix(1, 2), Y), AssertionFailed);
    }

    SECTION("gemm - 2x4 x 4x3") {
        auto A       = M(2, 4, 1., 2., 3., 4., 5., 6., 7., 8.);
        auto B       = M(4, 3, 9., 10., 11., 12., 13., 14., 15., 16., 17., 18., 19., 20.);
        auto C       = M(2, 3, 0., 0., 0., 0., 0., 0.);
        auto C_check = M(2, 3,
                         150., 160., 170.,   // row=0
                         366., 392., 418.);  // row=1

        linalg.gemm(A, B, C);

        EXPECT(equal_dense_matrix(C, C_check));
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);

    // Set dense linear algebra backend
    eckit::linalg::LinearAlgebraDense::backend(eckit::Resource<std::string>("-linearAlgebraDenseBackend", "generic"));

    return eckit::testing::run_tests(argc, argv, false);
}
