/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/runtime/Main.h"

#include "eckit/linalg/LinearAlgebra.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/testing/Test.h"
#include "./util.h"

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

template <class T>
bool equals(const T& a, const T& b) {
    return testing::make_view(a.data(), a.data() + a.size()) ==
           testing::make_view(b.data(), b.data() + b.size());
}

//-----------------------------------------------------------------------------

/// Test linear algebra interface

CASE ( "test backends" ) {
    using namespace linalg;

    Matrix A = M(2, 2, 1., -2., -4., 2.);
    SparseMatrix S(2, 4, {{0, 0, 2.}, {1, 2, 3.}});

    for (auto& back : {
         "generic",
 #ifdef ECKIT_HAVE_ARMADILLO
         "armadillo",
 #endif
 #ifdef ECKIT_HAVE_CUDA
         "cuda",
 #endif
 #ifdef ECKIT_HAVE_EIGEN
         "eigen",
 #endif
 #ifdef ECKIT_HAVE_MKL
         "mkl",
 #endif
 #ifdef ECKIT_HAVE_LAPACK
         "lapack",
 #endif
     }) {
        auto& linalg = LinearAlgebra::getBackend(back);

        Log::info() << linalg << " dot" << std::endl;
        {
            Vector a = V(3, 1., 2., 4.);

            EXPECT(linalg.dot(a, a) == 21.);
            EXPECT_THROWS_AS(linalg.dot(a, Vector(2)), AssertionFailed);
        }

        Log::info() << linalg << " gemv" << std::endl;
        {
            Vector y = V(2, -42., -42.);
            Vector z = V(2, 3., 0.);

            linalg.gemv(A, V(2, -1., -2.), y);
            EXPECT(equals(y, z));
            EXPECT_THROWS_AS(linalg.gemv(A, Vector(3), y), AssertionFailed);
        }

        Log::info() << linalg << " gemm" << std::endl;
        {
            Matrix Y = M(2, 2, -42., -42., -42., -42.);
            Matrix Z = M(2, 2, 9., -6., -12., 12.);

            linalg.gemm(A, A, Y);
            EXPECT(equals(Y, Z));
            EXPECT_THROWS_AS(linalg.gemm(A, Matrix(1, 2), Y), AssertionFailed);
        }

        Log::info() << linalg << " spmv" << std::endl;
        {
            Vector b = V(4, 5., 5., 5., 5.);
            Vector y = V(2, -42., -42.);
            Vector z = V(2,  10.,  15.);

            linalg.spmv(S, b, y);
            EXPECT(equals(y, z));
            EXPECT_THROWS_AS(linalg.spmv(S, Vector(3), y), AssertionFailed);
        }

        Log::info() << linalg << " spmm" << std::endl;
        {
            Matrix B = M(4, 2, 5., 7., 5., 7., 5., 7., 5., 7.);
            Matrix Y = M(2, 2, -42., -42., -42., -42.);
            Matrix Z = M(2, 2,  10.,  14.,  15.,  21.);

            linalg.spmm(S, B, Y);
            EXPECT(equals(Y, Z));
            EXPECT_THROWS_AS(linalg.spmm(S, Matrix(2, 2), Y), AssertionFailed);
        }
    }
}

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv) {
    using namespace eckit;

    Main::initialise(argc, argv);

    // Set linear algebra backend
    linalg::LinearAlgebra::backend(Resource<std::string>("-linearAlgebraBackend", "generic"));

    return testing::run_tests(argc, argv, false);
}
