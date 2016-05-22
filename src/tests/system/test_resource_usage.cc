/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <cstdlib>

#define BOOST_TEST_MODULE test_eckit_system

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/system/ResourceUsage.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_resource_usage )

BOOST_AUTO_TEST_CASE( test_eckit_resource_usage_0 )
{
    for(size_t i = 1; i < 5; ++i) {
        size_t chunk = 20*1024*1024;
        void *m = ::malloc(chunk);
        ::memset(m,0,chunk);
        system::ResourceUsage ru;
        BOOST_REQUIRE( ru.maxrss() >= chunk*i );
        std::cout << "Memory usage " << ru.maxrss() << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()

