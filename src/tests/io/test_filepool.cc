/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#define BOOST_TEST_MODULE test_eckit_io

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/FilePool.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/Buffer.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_io_filepool )

BOOST_AUTO_TEST_CASE( test_eckit_io_filepool_0 )
{
    Buffer buffer(50*1024);

    FilePool pool(1);

    BOOST_CHECK_EQUAL( pool.capacity(), 1 );

    BOOST_CHECK_NO_THROW( pool.resize(3); );

    BOOST_CHECK_EQUAL( pool.capacity(), 3 );

    DataHandle& foo = pool.checkout("foo.data");
    foo.write(buffer, buffer.size());

    BOOST_CHECK_EQUAL( pool.inUseSize(), 1 );

    DataHandle& bar = pool.checkout("bar.data");
    bar.write(buffer, buffer.size());

    BOOST_CHECK_EQUAL( pool.inUseSize(), 2 );

    std::cout << pool << std::endl;

    pool.checkin(foo);
    pool.checkin(bar);

    std::cout << pool << std::endl;

    BOOST_CHECK_EQUAL( pool.inUseSize(), 0 );

    DataHandle& foo2 = pool.checkout("foo.data");
    foo2.write(buffer, buffer.size());

    BOOST_CHECK_EQUAL( pool.inUseSize(), 1 );

    pool.checkin(foo);

    BOOST_CHECK_EQUAL( pool.inUseSize(), 0 );

    DataHandle& baz = pool.checkout("baz.data");
    baz.write(buffer, buffer.size());

    BOOST_CHECK_EQUAL( pool.inUseSize(), 1 );

}

BOOST_AUTO_TEST_SUITE_END()

