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
#include "eckit/memory/SharedPtr.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestCounted : public Tool {
public:

    TestCounted(int argc,char **argv): Tool(argc,argv) {}

    ~TestCounted() {}

    virtual void run();

    void test_default();
    void test_swap();

};

//-----------------------------------------------------------------------------

struct Foo : public Counted
{
    Foo( int in ) : i(in) {}
    int i;
};

void TestCounted::test_default()
{
    SharedPtr<Foo> pfoo;

    ASSERT( pfoo.null() );
    ASSERT( pfoo.get() == 0 );

    pfoo.reset( new Foo(10) );

    ASSERT( ! pfoo.null() );
    ASSERT( pfoo.unique() );
}

void TestCounted::test_swap()
{
    SharedPtr<Foo> p1( new Foo(10) );
    SharedPtr<Foo> p2( new Foo(5) );

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
    test_default();
    test_swap();
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestCounted mytest(argc,argv);
    mytest.start();
    return 0;
}

