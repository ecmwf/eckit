/*
 * (C) Copyright 1996-2016 ECMWF.
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

#define BOOST_TEST_MODULE test_eckit_la_factory

#include "ecbuild/boost_test_framework.h"

#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"

#include "eckit/linalg/LinearAlgebra.h"

#include "eckit/testing/Setup.h"

using namespace eckit::linalg;
using namespace eckit::testing;

namespace eckit {
namespace test {

/// Test linear algebra factory

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE(test_eckit_la_factory)

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(test_list) {
    std::ostringstream oss;
    LinearAlgebra::list(oss);
    BOOST_TEST_MESSAGE("Available linear algebra backends: " << oss.str());
    BOOST_CHECK_NE(oss.str().find("generic"), std::string::npos);
#ifdef ECKIT_HAVE_ARMADILLO
    BOOST_CHECK_NE(oss.str().find("armadillo"), std::string::npos);
#endif
#ifdef ECKIT_HAVE_CUDA
    BOOST_CHECK_NE(oss.str().find("cuda"), std::string::npos);
#endif
#ifdef ECKIT_HAVE_EIGEN
    BOOST_CHECK_NE(oss.str().find("eigen"), std::string::npos);
#endif
#ifdef ECKIT_HAVE_MKL
    BOOST_CHECK_NE(oss.str().find("mkl"), std::string::npos);
#endif
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(test_backend) {
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
    BOOST_CHECK_THROW(LinearAlgebra::backend("foo"), BadParameter);
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

}  // namespace test
} // namespace eckit
