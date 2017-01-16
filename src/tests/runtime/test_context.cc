/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_runtime

#include "ecbuild/boost_test_framework.h"

#include "eckit/log/Log.h"
#include "eckit/runtime/Main.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_resource )

BOOST_AUTO_TEST_CASE( test_default )
{
    /* log before context build */

    Log::info() << "logging before calling Context" << std::endl;

    /* setting context another time */


    Log::info()   << "logging after resetting behavior" << std::endl;
    Log::debug()  << "logging after resetting behavior" << std::endl;
    Log::warning()<< "logging after resetting behavior" << std::endl;
    Log::error()  << "logging after resetting behavior" << std::endl;
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
