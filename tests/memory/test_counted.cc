/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <memory>

#include "eckit/eckit.h"

#include "eckit/memory/Owned.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

struct FooLock : public OwnedLock
{
    using ptype = std::shared_ptr<FooLock>;

    FooLock( int in ) : i(in) {}
    int i;
};

struct FooNoLock : public OwnedNoLock
{
    using ptype = std::shared_ptr<FooNoLock>;

    FooNoLock( int in ) : i(in) {}
    int i;
};

template <typename F>
void TestDefault () {

    typename F::ptype p;
    EXPECT( !p );
    EXPECT( p.get() == 0 );

    p = std::make_shared<F>(10);

    EXPECT( p );
    EXPECT( p.unique() );

    p.reset();

    EXPECT( !p );
    EXPECT( p.use_count() == 0 );

}

template <typename F>
void TestCopy () {

    typename F::ptype p1;
    typename F::ptype p2;
    typename F::ptype p3;

    EXPECT( !p1 );
    EXPECT( p1.use_count() == 0 );

    p1 = std::make_shared<F>(10);

    EXPECT( p1->i == 10 );

    p1->i = 20;

    EXPECT( p1 != p2 );
    EXPECT( p1 != p3 );
    EXPECT( p2 == p3 );

    EXPECT(  p1 );
    EXPECT( !p2 );
    EXPECT( !p3 );

    EXPECT( p1.use_count() == 1 );
    EXPECT( p2.use_count() == 0 );
    EXPECT( p3.use_count() == 0 );

    p2 = p1;

    EXPECT( p1->i == 20 );
    EXPECT( p2->i == 20 );

    p1->i = 30;

    EXPECT( p1->i == 30 );
    EXPECT( p2->i == 30 );

    EXPECT( p1 == p2 );
    EXPECT( p1 != p3 );
    EXPECT( p2 != p3 );

    EXPECT(  p1 );
    EXPECT(  p2 );
    EXPECT( !p3 );

    EXPECT( p1.use_count() == 2 );
    EXPECT( p2.use_count() == 2 );
    EXPECT( p3.use_count() == 0 );

    p3 = p1;

    p1->i = 40;

    EXPECT( p1->i == 40 );
    EXPECT( p2->i == 40 );
    EXPECT( p3->i == 40 );

    EXPECT( p1 == p2 );
    EXPECT( p1 == p3 );
    EXPECT( p2 == p3 );

    EXPECT( p1 );
    EXPECT( p2 );
    EXPECT( p3 );

    EXPECT( p1.use_count() == 3 );
    EXPECT( p2.use_count() == 3 );
    EXPECT( p3.use_count() == 3 );

    p1.reset();

    EXPECT( !p1 );
    EXPECT(  p2 );
    EXPECT(  p3 );

    EXPECT( p1.use_count() == 0 );
    EXPECT( p2.use_count() == 2 );
    EXPECT( p3.use_count() == 2 );

    p2.reset();

    EXPECT( !p1 );
    EXPECT( !p2 );
    EXPECT(  p3 );

    EXPECT( p1.use_count() == 0 );
    EXPECT( p2.use_count() == 0 );
    EXPECT( p3.use_count() == 1 );

    p3.reset();

    EXPECT( !p1 );
    EXPECT( !p2 );
    EXPECT( !p3 );

    EXPECT( p1.use_count() == 0 );
    EXPECT( p2.use_count() == 0 );
    EXPECT( p3.use_count() == 0 );

}

template <typename F>
void TestReset( )
{
    typename F::ptype p;

    EXPECT( !p );

    p.reset();

    EXPECT( !p );

    p = std::make_shared<F>(10);

    EXPECT( p );

    p.reset();

    EXPECT( !p );
}

template <typename F>
void TestSwap( )
{
    typename F::ptype p1( new F(10) );
    typename F::ptype p2( new F(5) );

    EXPECT( p1 );
    EXPECT( p2 );

    EXPECT( p1.unique() );
    EXPECT( p2.unique() );


    EXPECT( p1->i == 10 );
    EXPECT( p2->i == 5 );

    p1.swap(p2);

    EXPECT( p1->i == 5 );
    EXPECT( p2->i == 10 );

    EXPECT( p1.unique() );
    EXPECT( p2.unique() );

    EXPECT( p1 );
    EXPECT( p2 );
}

//-----------------------------------------------------------------------------

CASE ( "test_default" ) {
    SECTION( "Type FooLock") {
        TestDefault< FooLock > ();
    }
    SECTION( "Type FooNoLock") {
        TestDefault< FooNoLock > ();
    }
}

CASE ( "test_copy" ) {
    SECTION( "Type FooLock") {
        TestCopy< FooLock > ();
    }
    SECTION( "Type FooNoLock") {
        TestCopy< FooNoLock > ();
    }
}

CASE ( "test_reset" ) {
    SECTION( "Type FooLock") {
        TestReset< FooLock > ();
    }
    SECTION( "Type FooNoLock") {
        TestReset< FooNoLock > ();
    }
}

CASE ( "test_swap" ) {
    SECTION( "Type FooLock") {
        TestSwap< FooLock > ();
    }
    SECTION( "Type FooNoLock") {
        TestSwap< FooNoLock > ();
    }
}

//-----------------------------------------------------------------------------

} // namespace test
} // namespace eckit

int main(int argc,char **argv)
{
    return run_tests ( argc, argv );
}
