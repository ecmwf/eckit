/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/eckit.h"
#include "eckit/linalg/LinearAlgebraDense.h"
#include "eckit/linalg/LinearAlgebraSparse.h"
#include "eckit/testing/Test.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("list") {
    using linalg::LinearAlgebraDense;
    using linalg::LinearAlgebraSparse;

    auto dense_backends = {
        "generic",
#ifdef eckit_HAVE_ARMADILLO
        "armadillo",
#endif
#ifdef eckit_HAVE_CUDA
        "cuda",
#endif
#ifdef eckit_HAVE_EIGEN
        "eigen",
#endif
#ifdef eckit_HAVE_LAPACK
        "lapack",
#endif
#ifdef eckit_HAVE_MKL
#if eckit_HAVE_MKL
        "mkl",
#endif
#endif
#ifdef eckit_HAVE_VIENNACL
        "viennacl",
#endif
#ifdef eckit_HAVE_OMP
        "openmp",
#endif
    };

    auto sparse_backends = {
        "generic",
#ifdef eckit_HAVE_CUDA
        "cuda",
#endif
#ifdef eckit_HAVE_EIGEN
        "eigen",
#endif
#ifdef eckit_HAVE_MKL
#if eckit_HAVE_MKL
        "mkl",
#endif
#endif
#ifdef eckit_HAVE_VIENNACL
        "viennacl",
#endif
#ifdef eckit_HAVE_OMP
        "openmp",
#endif
    };

    SECTION("dense backends") {
        std::ostringstream oss;
        LinearAlgebraDense::list(oss);
        Log::info() << "Available backends: " << oss.str() << std::endl;

        for (std::string name : dense_backends) {
            Log::info() << "Looking for '" << name << "'" << std::endl;
            EXPECT(oss.str().find(name) != std::string::npos);

            Log::info() << "Switching to '" << name << "'" << std::endl;
            LinearAlgebraDense::backend(name);
            EXPECT(LinearAlgebraDense::backend().name() == name);
        }

        EXPECT_THROWS_AS(LinearAlgebraDense::backend("foo"), BadParameter);
    }

    SECTION("sparse backends") {
        std::ostringstream oss;
        LinearAlgebraSparse::list(oss);
        Log::info() << "Available backends: " << oss.str() << std::endl;

        for (std::string name : sparse_backends) {
            Log::info() << "Looking for '" << name << "'" << std::endl;
            EXPECT(oss.str().find(name) != std::string::npos);

            Log::info() << "Switching to '" << name << "'" << std::endl;
            LinearAlgebraSparse::backend(name);
            EXPECT(LinearAlgebraSparse::backend().name() == name);
        }

        EXPECT_THROWS_AS(LinearAlgebraSparse::backend("foo"), BadParameter);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
