/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_maths_eigen

#include "ecbuild/boost_test_framework.h"

#include "eckit/maths/Eigen.h"

//-----------------------------------------------------------------------------

namespace eckit_test {}

//-----------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_resource )

BOOST_AUTO_TEST_CASE( test_default )
{
    Eigen::Vector3d v1;
    Eigen::Vector3d v2;
    Eigen::Vector3d r;

    r = v2 - v1;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
