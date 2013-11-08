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
    /// Tests a scalar-scalar recursive reduction
    void test_reduce_recursive_scalars();
    /// Tests a prod(s,v,v) reduction
    void test_reduce_prodadd();
    /// Tests operators like s * ( v + v )
    void test_operators();

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

    test_reduce_scalars();
    test_reduce_recursive_scalars();
    test_reduce_prodadd();
    test_operators();

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

    // signature before reducing
    ASSERT( e->signature() == "Add(Prod(Add(s,s),v),Prod(s,v))" );

    // eval() first calls reduce internally
    ASSERT( e->eval()->str() == "Vector(58, 58, 58, 58, 58, 58, 58, 58, 58, 58)" );

    // signature after reducing
    ASSERT( e->signature() == "Add(Prod(s,v),Prod(s,v))" );
}

void TestExp::test_reduce_recursive_scalars()
{
    ExpPtr c1 = maths::add(a_,b_);
    ExpPtr c2 = maths::add(c1,c1);
    ExpPtr c3 = maths::add(c2,c2);
    ExpPtr c4 = maths::add(c3,c3);
    ExpPtr e  = maths::add(c4,c4);

    ASSERT( e->ret_signature() == Scalar::sig() );

    // signature before reducing
    ASSERT( e->signature() == "Add(Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))),Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))))" );

    // got fully reduced to a scalar
    ASSERT( e->reduce()->signature() == Scalar::sig() );

    // correct reduction
    ASSERT( e->eval()->str() == "Scalar(96)" );

}

void TestExp::test_reduce_prodadd()
{
    ExpPtr e0 = maths::prod( a_, maths::add(y_, x_ ) );
    ASSERT( e0->reduce()->signature() == "ProdAdd(s,v,v)" );
    ASSERT( e0->eval()->str() == "Vector(24, 24, 24, 24, 24, 24, 24, 24, 24, 24)" );

    ExpPtr e1 = maths::prod( x_, maths::add(y_, x_ ) );
    ASSERT( e1->reduce()->signature() == "ProdAdd(v,v,v)" );
    ASSERT( e1->eval()->str() == "Vector(60, 60, 60, 60, 60, 60, 60, 60, 60, 60)" );

    // involves also reducing the scalar-scalar
    ExpPtr e2 = maths::prod( maths::prod(a_,b_), maths::add(y_, x_ ) );
    ASSERT( e2->reduce()->signature() == "ProdAdd(s,v,v)" );
    ASSERT( e2->eval()->str() == "Vector(96, 96, 96, 96, 96, 96, 96, 96, 96, 96)" );
}

void TestExp::test_operators()
{
    ExpPtr e0 =  a_ * ( y_ +  x_ );

    ASSERT( e0->reduce()->signature() == "ProdAdd(s,v,v)" );
    ASSERT( e0->eval()->str() == "Vector(24, 24, 24, 24, 24, 24, 24, 24, 24, 24)" );

    ExpPtr e1 =  maths::vector( 10, 10. ) / maths::vector( 10, 5. );
    ASSERT( e1->eval()->str() == "Vector(2, 2, 2, 2, 2, 2, 2, 2, 2, 2)" );

    ExpPtr e2 =  maths::vector( 10, 13. ) - maths::vector( 10, 5. );
    ASSERT( e2->eval()->str() == "Vector(8, 8, 8, 8, 8, 8, 8, 8, 8, 8)" );
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

