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

//-----------------------------------------------------------------------------

class TestCounted : public Tool {
public:

    TestCounted(int argc,char **argv): Tool(argc,argv) {}

    ~TestCounted() {}

    virtual void run();

    template< typename F >
    void test_default();

    template< typename F >
    void test_release();

    template< typename F >
    void test_copy();

    template< typename F >
    void test_swap();

};

//-----------------------------------------------------------------------------

template< typename F >
void TestCounted::test_default()
{
//    std::cout << ">>> test_default()" << std::endl;

    typename F::ptype p;

    ASSERT( p.null() );
    ASSERT( p.get() == 0 );

    p.reset( new F(10) );

    ASSERT( ! p.null() );
    ASSERT( p.unique() );

    p.release();

    ASSERT( p.null() );
    ASSERT( p.owners() == 0 );
}

template< typename F >
void TestCounted::test_copy()
{
//    std::cout << ">>> test_copy()" << std::endl;

    typename F::ptype p1;
    typename F::ptype p2;
    typename F::ptype p3;

    ASSERT( p1.null() );
    ASSERT( p1.owners() == 0 );

    p1.reset( new F(10) );

    ASSERT( p1->i == 10 );

    p1->i = 20;

    ASSERT( p1 != p2 );
    ASSERT( p1 != p3 );
    ASSERT( p2 == p3 );

    ASSERT( ! p1.null() );
    ASSERT( p2.null() );
    ASSERT( p3.null() );

    ASSERT( p1.owners() == 1 );
    ASSERT( p2.owners() == 0 );
    ASSERT( p3.owners() == 0 );

    p2.reset(p1);

    ASSERT( p1->i == 20 );
    ASSERT( p2->i == 20 );

    p1->i = 30;

    ASSERT( p1->i == 30 );
    ASSERT( p2->i == 30 );

    ASSERT( p1 == p2 );
    ASSERT( p1 != p3 );
    ASSERT( p2 != p3 );

    ASSERT( ! p1.null() );
    ASSERT( ! p2.null() );
    ASSERT( p3.null() );

    ASSERT( p1.owners() == 2 );
    ASSERT( p2.owners() == 2 );
    ASSERT( p3.owners() == 0 );

    p3 = p1;

    p1->i = 40;

    ASSERT( p1->i == 40 );
    ASSERT( p2->i == 40 );
    ASSERT( p3->i == 40 );

    ASSERT( p1 == p2 );
    ASSERT( p1 == p3 );
    ASSERT( p2 == p3 );

    ASSERT( ! p1.null() );
    ASSERT( ! p2.null() );
    ASSERT( ! p3.null() );

    ASSERT( p1.owners() == 3 );
    ASSERT( p2.owners() == 3 );
    ASSERT( p3.owners() == 3 );

    p1.release();

    ASSERT( p1.null() );
    ASSERT( ! p2.null() );
    ASSERT( ! p3.null() );

    ASSERT( p1.owners() == 0 );
    ASSERT( p2.owners() == 2 );
    ASSERT( p3.owners() == 2 );

    p2.release();

    ASSERT( p1.null() );
    ASSERT( p2.null() );
    ASSERT( ! p3.null() );

    ASSERT( p1.owners() == 0 );
    ASSERT( p2.owners() == 0 );
    ASSERT( p3.owners() == 1 );

    p3.release();

    ASSERT( p1.null() );
    ASSERT( p2.null() );
    ASSERT( p3.null() );

    ASSERT( p1.owners() == 0 );
    ASSERT( p2.owners() == 0 );
    ASSERT( p3.owners() == 0 );
}

template< typename F >
void TestCounted::test_release()
{
//    std::cout << ">>> test_release()" << std::endl;

    typename F::ptype p;

    ASSERT( p.null() );

    p.release();

    ASSERT( p.null() );

    p.reset( new F(10) );

    ASSERT( ! p.null() );

    p.release();

    ASSERT( p.null() );
}

template< typename F >
void TestCounted::test_swap()
{
//    std::cout << ">>> test_swap()" << std::endl;

    typename F::ptype p1( new F(10) );
    typename F::ptype p2( new F(5) );

    ASSERT( ! p1.null() );
    ASSERT( ! p2.null() );

    ASSERT( p1.unique() );
    ASSERT( p2.unique() );


    ASSERT( p1->i == 10 );
    ASSERT( p2->i == 5 );

    p1.swap(p2);

    ASSERT( p1->i == 5 );
    ASSERT( p2->i == 10 );

    ASSERT( p1.unique() );
    ASSERT( p2.unique() );

    ASSERT( ! p1.null() );
    ASSERT( ! p2.null() );
}

//-----------------------------------------------------------------------------
            
void TestCounted::run()
{
    test_default<FooLock>();
    test_copy<FooLock>();
    test_release<FooLock>();
    test_swap<FooLock>();

    test_default<FooNoLock>();
    test_copy<FooNoLock>();
    test_release<FooNoLock>();
    test_swap<FooNoLock>();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    using namespace eckit_test;

    TestCounted mytest(argc,argv);
    mytest.start();

    return 0;
}

