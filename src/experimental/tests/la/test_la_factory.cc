/*
 * (C) Copyright 1996-2015 ECMWF.
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
#include "experimental/eckit/la/LinearAlgebraFactory.h"

//-----------------------------------------------------------------------------

using namespace eckit::la;

namespace eckit {
namespace test {

/// Test linear algebra factory

BOOST_AUTO_TEST_SUITE(test_eckit_la_factory)

BOOST_AUTO_TEST_CASE(test_list) {
    std::ostringstream oss;
    LinearAlgebraFactory::list(oss);
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

BOOST_AUTO_TEST_CASE(test_set) {
    BOOST_CHECK(LinearAlgebraFactory::get());
    LinearAlgebraFactory::set("generic");
    BOOST_CHECK(LinearAlgebraFactory::get());
#ifdef ECKIT_HAVE_ARMADILLO
    LinearAlgebraFactory::set("armadillo");
    BOOST_CHECK(LinearAlgebraFactory::get());
#endif
#ifdef ECKIT_HAVE_CUDA
    LinearAlgebraFactory::set("cuda");
    BOOST_CHECK(LinearAlgebraFactory::get());
#endif
#ifdef ECKIT_HAVE_EIGEN
    LinearAlgebraFactory::set("eigen");
    BOOST_CHECK(LinearAlgebraFactory::get());
#endif
#ifdef ECKIT_HAVE_MKL
    LinearAlgebraFactory::set("mkl");
    BOOST_CHECK(LinearAlgebraFactory::get());
#endif
    BOOST_CHECK_THROW(LinearAlgebraFactory::set("foo"), BadParameter);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace test
}  // namespace eckit
