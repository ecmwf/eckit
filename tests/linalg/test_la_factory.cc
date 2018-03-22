/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   test_la_factory.cc
/// @author Florian Rathgeber
/// @date   July 2015


#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/LinearAlgebra.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;
using namespace eckit::linalg;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

CASE("test_list") {
    std::ostringstream oss;
    LinearAlgebra::list(oss);
    Log::info() << "Available linear algebra backends: " << oss.str() << std::endl;
    EXPECT(oss.str().find("generic") != std::string::npos);
#ifdef ECKIT_HAVE_ARMADILLO
    EXPECT(oss.str().find("armadillo") != std::string::npos);
#endif
#ifdef ECKIT_HAVE_CUDA
    EXPECT(oss.str().find("cuda") != std::string::npos);
#endif
#ifdef ECKIT_HAVE_EIGEN
    EXPECT(oss.str().find("eigen") != std::string::npos);
#endif
#ifdef ECKIT_HAVE_MKL
    EXPECT(oss.str().find("mkl") != std::string::npos);
#endif
}

CASE("test_backend") {
    LinearAlgebra::backend();
    LinearAlgebra::backend("generic");
    LinearAlgebra::backend();
#ifdef ECKIT_HAVE_ARMADILLO
    LinearAlgebra::backend("armadillo");
    LinearAlgebra::backend();
#endif
#ifdef ECKIT_HAVE_CUDA
    LinearAlgebra::backend("cuda");
    LinearAlgebra::backend();
#endif
#ifdef ECKIT_HAVE_EIGEN
    LinearAlgebra::backend("eigen");
    LinearAlgebra::backend();
#endif
#ifdef ECKIT_HAVE_MKL
    LinearAlgebra::backend("mkl");
    LinearAlgebra::backend();
#endif
    EXPECT_THROWS_AS(LinearAlgebra::backend("foo"), BadParameter);
}

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv)
{
    return run_tests ( argc, argv );
}
