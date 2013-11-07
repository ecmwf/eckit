/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/maths/Exp.h"
#include "eckit/maths/Scalar.h"
#include "eckit/maths/Vector.h"
#include "eckit/maths/BinaryFunc.h"

using namespace eckit;
using namespace eckit::maths;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

class TestExp : public Tool {
public:

    TestExp(int argc,char **argv): Tool(argc,argv) {}

    virtual void run();

    void setup();

    /// Tests a scalar-scalar reduction within another more complex expression
    void test_reduce_scalars();
    /// Tests a prod(s,v,v) reduction
    void test_reduce_prodadd();

    void teardown();

protected:
    ExpPtr a_;
    ExpPtr b_;
    ExpPtr x_;
    ExpPtr y_;
};

void TestExp::run()
{
    setup();

//    test_reduce_scalars();
    test_reduce_prodadd();

    teardown();
}

void TestExp::setup()
{
    a_ = maths::scalar( 2. );
    b_ = maths::scalar( 4. );
    x_ = maths::vector( 10, 5. );
    y_ = maths::vector( 10, 7. );
}

void TestExp::teardown() {}

void TestExp::test_reduce_scalars()
{
    ExpPtr c = maths::add(a_,b_); // scalar-scalar

    ExpPtr e = maths::add( prod( c , x_ ) , prod( b_, y_ ));

    ASSERT( e->ret_signature() == Vector::sig() );

    std::cout << "before expr " << *e << std::endl;
    std::cout << "before signature: " << e->signature() << std::endl;

    std::cout << "reduced signature: " << e->reduce()->signature() << std::endl;

    std::cout << "after expr " << *e << std::endl;
    std::cout << "after signature: " << e->signature() << std::endl;

    std::cout << *(e->eval()) << std::endl;

    std::cout << "reduced signature: " << e->signature() << std::endl;
}

void TestExp::test_reduce_prodadd()
{
    ExpPtr e0 = maths::prod( a_, maths::add(y_, x_ ) );
    ASSERT( e0->reduce()->signature() == "ProdAdd(s,v,v)" );

    ExpPtr e1 = maths::prod( x_, maths::add(y_, x_ ) );
    ASSERT( e1->reduce()->signature() == "ProdAdd(v,v,v)" );

    // involves also reducing the scalar-scalar
    ExpPtr e2 = maths::prod( maths::prod(a_,b_), maths::add(y_, x_ ) );
    ASSERT( e2->reduce()->signature() == "ProdAdd(s,v,v)" );
}

//-----------------------------------------------------------------------------

} // namespace eckit_test

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    eckit_test::TestExp mytest(argc,argv);
    mytest.start();
    return 0;
}

