/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_types

#include "ecbuild/boost_test_framework.h"

#include "eckit/types/FixedString.h"

using namespace std;
using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_types_fixedstring )

BOOST_AUTO_TEST_CASE( test_eckit_types_fixedstring_boom )
{
    BOOST_CHECK(true);
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
