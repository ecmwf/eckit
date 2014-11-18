/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/eckit_config.h"

#define BOOST_TEST_MODULE test_eckit_memory

#include "ecbuild/boost_test_framework.h"

#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

#include "eckit/memory/Counted.h"
#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

struct FooLock : public OwnedLock
{
    typedef SharedPtr<FooLock> ptype;

    FooLock( int in ) : i(in) {}
    int i;
};

struct FooNoLock : public OwnedNoLock
{
    typedef SharedPtr<FooNoLock> ptype;

    FooNoLock( int in ) : i(in) {}
    int i;
};

}

typedef boost::mpl::list< eckit_test::FooLock, eckit_test::FooNoLock > test_types;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_memory_counted )

BOOST_AUTO_TEST_CASE_TEMPLATE( test_default, F, test_types )
{
    typename F::ptype p;

    BOOST_CHECK( !p );
    BOOST_CHECK( p.get() == 0 );

    p.reset( new F(10) );

    BOOST_CHECK( p );
    BOOST_CHECK( p.unique() );

    p.release();

    BOOST_CHECK( !p );
    BOOST_CHECK( p.owners() == 0 );
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_copy, F, test_types )
{
    typename F::ptype p1;
    typename F::ptype p2;
    typename F::ptype p3;

    BOOST_CHECK( !p1 );
    BOOST_CHECK( p1.owners() == 0 );

    p1.reset( new F(10) );

    BOOST_CHECK( p1->i == 10 );

    p1->i = 20;

    BOOST_CHECK( p1 != p2 );
    BOOST_CHECK( p1 != p3 );
    BOOST_CHECK( p2 == p3 );

    BOOST_CHECK(  p1 );
    BOOST_CHECK( !p2 );
    BOOST_CHECK( !p3 );

    BOOST_CHECK( p1.owners() == 1 );
    BOOST_CHECK( p2.owners() == 0 );
    BOOST_CHECK( p3.owners() == 0 );

    p2.reset(p1);

    BOOST_CHECK( p1->i == 20 );
    BOOST_CHECK( p2->i == 20 );

    p1->i = 30;

    BOOST_CHECK( p1->i == 30 );
    BOOST_CHECK( p2->i == 30 );

    BOOST_CHECK( p1 == p2 );
    BOOST_CHECK( p1 != p3 );
    BOOST_CHECK( p2 != p3 );

    BOOST_CHECK(  p1 );
    BOOST_CHECK(  p2 );
    BOOST_CHECK( !p3 );

    BOOST_CHECK( p1.owners() == 2 );
    BOOST_CHECK( p2.owners() == 2 );
    BOOST_CHECK( p3.owners() == 0 );

    p3 = p1;

    p1->i = 40;

    BOOST_CHECK( p1->i == 40 );
    BOOST_CHECK( p2->i == 40 );
    BOOST_CHECK( p3->i == 40 );

    BOOST_CHECK( p1 == p2 );
    BOOST_CHECK( p1 == p3 );
    BOOST_CHECK( p2 == p3 );

    BOOST_CHECK( p1 );
    BOOST_CHECK( p2 );
    BOOST_CHECK( p3 );

    BOOST_CHECK( p1.owners() == 3 );
    BOOST_CHECK( p2.owners() == 3 );
    BOOST_CHECK( p3.owners() == 3 );

    p1.release();

    BOOST_CHECK( !p1 );
    BOOST_CHECK(  p2 );
    BOOST_CHECK(  p3 );

    BOOST_CHECK( p1.owners() == 0 );
    BOOST_CHECK( p2.owners() == 2 );
    BOOST_CHECK( p3.owners() == 2 );

    p2.release();

    BOOST_CHECK( !p1 );
    BOOST_CHECK( !p2 );
    BOOST_CHECK(  p3 );

    BOOST_CHECK( p1.owners() == 0 );
    BOOST_CHECK( p2.owners() == 0 );
    BOOST_CHECK( p3.owners() == 1 );

    p3.release();

    BOOST_CHECK( !p1 );
    BOOST_CHECK( !p2 );
    BOOST_CHECK( !p3 );

    BOOST_CHECK( p1.owners() == 0 );
    BOOST_CHECK( p2.owners() == 0 );
    BOOST_CHECK( p3.owners() == 0 );
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_release, F, test_types )
{
    typename F::ptype p;

    BOOST_CHECK( !p );

    p.release();

    BOOST_CHECK( !p );

    p.reset( new F(10) );

    BOOST_CHECK( p );

    p.release();

    BOOST_CHECK( !p );
}

BOOST_AUTO_TEST_CASE_TEMPLATE( test_swap, F, test_types )
{
    typename F::ptype p1( new F(10) );
    typename F::ptype p2( new F(5) );

    BOOST_CHECK( p1 );
    BOOST_CHECK( p2 );

    BOOST_CHECK( p1.unique() );
    BOOST_CHECK( p2.unique() );


    BOOST_CHECK( p1->i == 10 );
    BOOST_CHECK( p2->i == 5 );

    p1.swap(p2);

    BOOST_CHECK( p1->i == 5 );
    BOOST_CHECK( p2->i == 10 );

    BOOST_CHECK( p1.unique() );
    BOOST_CHECK( p2.unique() );

    BOOST_CHECK( p1 );
    BOOST_CHECK( p2 );
}

BOOST_AUTO_TEST_SUITE_END()
