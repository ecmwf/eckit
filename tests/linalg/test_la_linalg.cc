/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/runtime/Main.h"

#include "eckit/linalg/LinearAlgebra.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"
#include "./util.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

struct Fixture {

    Fixture() : x(V(3, 1., 2., 4.)),
                A(M(2, 2, 1., -2., -4., 2.)),
                linalg(LinearAlgebra::backend()) {}

    Vector x;
    Matrix A;
    const LinearAlgebra& linalg;
};


template <class T>
void test(const T& v, const T& r) {
    EXPECT( make_view(v.data(), v.data() + v.size()) == make_view(r.data(), r.data() + r.size()));
}

//----------------------------------------------------------------------------------------------------------------------

/// Test linear algebra interface

CASE ( "test_eckit_la_sparse" ) {

    SETUP("Fixture") {
        Fixture F;

        SECTION("test_dot") {
            EXPECT(F.linalg.dot(F.x, F.x) == 21.);
            Log::info() << "dot of vectors of different sizes should fail" << std::endl;
            EXPECT_THROWS_AS(F.linalg.dot(F.x, Vector(2)), AssertionFailed);
        }

        SECTION("test_gemv") {
            Vector y(2);
            F.linalg.gemv(F.A, V(2, -1., -2.), y);
            test(y, V(2, 3., 0.));
            Log::info() << "gemv of matrix and vector of nonmatching sizes should fail" << std::endl;
            EXPECT_THROWS_AS(F.linalg.gemv(F.A, F.x, y), AssertionFailed);
        }

        SECTION("test_gemm") {
            Matrix B(2, 2);
            F.linalg.gemm(F.A, F.A, B);
            test(B, M(2, 2, 9., -6., -12., 12.));
            Log::info() << "gemm of matrices of nonmatching sizes should fail" << std::endl;
            EXPECT_THROWS_AS(F.linalg.gemm(F.A, Matrix(1, 2), B), AssertionFailed);
        }
    }
}



//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv)
{
    eckit::Main::initialise( argc, argv );
    // Set linear algebra backend
    LinearAlgebra::backend(Resource<std::string>("-linearAlgebraBackend", "generic"));

    return run_tests ( argc, argv, false );
}
