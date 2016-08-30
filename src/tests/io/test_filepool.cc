/*
 * (C) Copyright 1996-2016 ECMWF.
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

#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_io_filepool )

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_io_filepool_0 ) {
    Buffer buffer(50*1024);

    FilePool pool(1);

    // pool sized 1

    BOOST_CHECK_EQUAL( pool.capacity(), 1 );
    BOOST_CHECK_EQUAL( pool.size(),     0 );

    pool.capacity(3);

    // pool is larger

    BOOST_CHECK_EQUAL( pool.capacity(), 3 );
    BOOST_CHECK_EQUAL( pool.size(),     0 );

    DataHandle* foo = pool.checkout("foo.data");
    foo->write(buffer, buffer.size());

    // foo is in use

    BOOST_CHECK_EQUAL( pool.usage(), 1 );
    BOOST_CHECK_EQUAL( pool.size(),  0 );

    DataHandle* bar = pool.checkout("bar.data");
    bar->write(buffer, buffer.size());

    // foo + bar are in use

    BOOST_CHECK_EQUAL( pool.usage(), 2 );
    BOOST_CHECK_EQUAL( pool.size(),  0 );

    std::cout << pool << std::endl;

    pool.checkin(foo);
    pool.checkin(bar);

    std::cout << pool << std::endl;

    // foo + bar are in pool, none in use

    BOOST_CHECK_EQUAL( pool.usage(), 0 );
    BOOST_CHECK_EQUAL( pool.size(),  2 );

    DataHandle* foo2 = pool.checkout("foo.data");
    foo2->write(buffer, buffer.size());

    // foo in use again, bar in pool

    BOOST_CHECK_EQUAL( pool.usage(), 1 );
    BOOST_CHECK_EQUAL( pool.size(),  1 );

    pool.checkin(foo);

    // foo + bar are again in pool, none in use

    BOOST_CHECK_EQUAL( pool.usage(), 0 );
    BOOST_CHECK_EQUAL( pool.size(),  2 );

    DataHandle* baz = pool.checkout("baz.data");
    baz->write(buffer, buffer.size());

    // foo + bar in pool, baz in use

    BOOST_CHECK_EQUAL( pool.usage(), 1 );
    BOOST_CHECK_EQUAL( pool.size(),  2 );

    pool.checkin(baz);

    // baz + foo + bar in pool, none in use

    BOOST_CHECK_EQUAL( pool.usage(), 0 );
    BOOST_CHECK_EQUAL( pool.size(),  3 );

    DataHandle* marco = pool.checkout("marco.data");
    marco->write(buffer, buffer.size());

    // baz + foo + bar in pool, marco in use

    BOOST_CHECK_EQUAL( pool.usage(), 1 );
    BOOST_CHECK_EQUAL( pool.size(),  3 );

    pool.checkin(marco);

    // marco + foo + baz in pool, none in use, bar was purged (closed)

    BOOST_CHECK_EQUAL( pool.usage(), 0 );
    BOOST_CHECK_EQUAL( pool.size(),  3 );

    std::cout << pool << std::endl;

    pool.capacity(1);

    // marco in pool, none in use, foo + baz were purged (closed) due to reduced capacity

    BOOST_CHECK_EQUAL( pool.usage(), 0 );
    BOOST_CHECK_EQUAL( pool.size(),  1 );

    std::cout << pool << std::endl;

    DataHandle* polo = pool.checkout("polo.data");
    polo->write(buffer, buffer.size());

    // marco in pool, polo is in use

    BOOST_CHECK_EQUAL( pool.usage(), 1 );
    BOOST_CHECK_EQUAL( pool.size(),  1 );

    std::cout << pool << std::endl;

    pool.checkin(polo);

    // polo pushes marco out and remains alone in pool

    BOOST_CHECK_EQUAL( pool.usage(), 0 );
    BOOST_CHECK_EQUAL( pool.size(),  1 );

    std::cout << pool << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
