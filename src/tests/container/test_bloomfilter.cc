/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE eckit_test_bloomfilter

#include "ecbuild/boost_test_framework.h"

#include "eckit/container/BloomFilter.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_container_bloomfilter )

BOOST_AUTO_TEST_CASE( test_eckit_container_bloomfilter_insert )
{

    BloomFilter<std::string> f(1024);

    BOOST_CHECK(f.empty());
    BOOST_CHECK(!f.contains("hello there"));
    BOOST_CHECK(!f.contains("hello there again"));

    f.insert("hello there");

    BOOST_CHECK(f.contains("hello there"));
    BOOST_CHECK(!f.contains("hello there again"));
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
