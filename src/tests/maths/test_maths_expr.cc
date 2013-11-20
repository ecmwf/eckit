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

#include "eckit/maths/BinaryFunc.h"
#include "eckit/maths/Count.h"
#include "eckit/maths/Exp.h"
#include "eckit/maths/FMap.h"
#include "eckit/maths/Reduce.h"
#include "eckit/maths/List.h"
#include "eckit/maths/Scalar.h"
#include "eckit/maths/UnaryFunc.h"
#include "eckit/maths/Vector.h"
#include "eckit/maths/ZipWith.h"

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
    void test_optimise_scalars();
    /// Tests a scalar-scalar recursive reduction
    void test_optimise_recursive_scalars();
    /// Tests a prod(s,v,v) reduction
    void test_optimise_prodadd();
    /// Tests operators like s * ( v + v )
    void test_operators();
    /// Tests list functions
    void test_list();
    /// Tests fmap expression
    void test_fmap();
    /// Tests reduce expression
    void test_reduce();
    /// Tests zipwith expression
    void test_zipwith();

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

    test_optimise_scalars();
    test_optimise_recursive_scalars();
    test_optimise_prodadd();
    test_operators();
    test_list();
    test_fmap();
    test_reduce();
    test_zipwith();

    teardown();
}

void TestExp::setup()
{
    DBG;

    a_ = maths::scalar( 2. );
    b_ = maths::scalar( 4. );
    x_ = maths::vector( 10, 5. );
    y_ = maths::vector( 10, 7. );
}

void TestExp::teardown()
{
    DBG;
}

void TestExp::test_optimise_scalars()
{
    DBG;

    ExpPtr c = maths::add(a_,b_); // scalar-scalar

    ExpPtr e = maths::add( prod( c , x_ ) , prod( b_, y_ ));

    DBG;

    ASSERT( e->ret_signature() == Vector::sig() );

    DBG;
    // signature before reducing
    ASSERT( e->signature() == "Add(Prod(Add(s,s),v),Prod(s,v))" );

    DBG;
    // eval() first calls optimise internally
    ASSERT( e->eval()->str() == "Vector(58, 58, 58, 58, 58, 58, 58, 58, 58, 58)" );

    DBG;
    // signature after reducing
    ASSERT( e->signature() == "Add(Prod(s,v),Prod(s,v))" );
}

void TestExp::test_optimise_recursive_scalars()
{
    DBG;

    ExpPtr c1 = maths::add(a_,b_);
    ExpPtr c2 = maths::add(c1,c1);
    ExpPtr c3 = maths::add(c2,c2);
    ExpPtr c4 = maths::add(c3,c3);
    ExpPtr e  = maths::add(c4,c4);

    ASSERT( e->ret_signature() == Scalar::sig() );

    // signature before reducing
    ASSERT( e->signature() == "Add(Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))),Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))))" );

    // got fully optimised to a scalar
    ASSERT( e->optimise()->signature() == Scalar::sig() );

    // correct reduction
    ASSERT( e->eval()->str() == "Scalar(96)" );

}

void TestExp::test_optimise_prodadd()
{
    DBG;

    ExpPtr e0 = maths::prod( a_, maths::add(y_, x_ ) );
    ASSERT( e0->optimise()->signature() == "ProdAdd(s,v,v)" );
    ASSERT( e0->eval()->str() == "Vector(24, 24, 24, 24, 24, 24, 24, 24, 24, 24)" );

    ExpPtr e1 = maths::prod( x_, maths::add(y_, x_ ) );
    ASSERT( e1->optimise()->signature() == "ProdAdd(v,v,v)" );
    ASSERT( e1->eval()->str() == "Vector(60, 60, 60, 60, 60, 60, 60, 60, 60, 60)" );

    // involves also reducing the scalar-scalar
    ExpPtr e2 = maths::prod( maths::prod(a_,b_), maths::add(y_, x_ ) );
    ASSERT( e2->optimise()->signature() == "ProdAdd(s,v,v)" );
    ASSERT( e2->eval()->str() == "Vector(96, 96, 96, 96, 96, 96, 96, 96, 96, 96)" );
}

void TestExp::test_operators()
{
    DBG;

    ExpPtr e0 =  a_ * ( y_ +  x_ );

    ASSERT( e0->optimise()->signature() == "ProdAdd(s,v,v)" );
    ASSERT( e0->eval()->str() == "Vector(24, 24, 24, 24, 24, 24, 24, 24, 24, 24)" );

    ExpPtr e1 =  maths::vector( 10, 10. ) / maths::vector( 10, 5. );
    ASSERT( e1->eval()->str() == "Vector(2, 2, 2, 2, 2, 2, 2, 2, 2, 2)" );

    ExpPtr e2 =  maths::vector( 10, 13. ) - maths::vector( 10, 5. );
    ASSERT( e2->eval()->str() == "Vector(8, 8, 8, 8, 8, 8, 8, 8, 8, 8)" );
}

void TestExp::test_list()
{
    DBG;

    ExpPtr l0 =  maths::list( y_ , x_ );

    ASSERT( l0->eval()->str() == "List(Vector(7, 7, 7, 7, 7, 7, 7, 7, 7, 7), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5))" );
    ASSERT( l0->as<List>()->size() == 2 );

    ExpPtr l1 = maths::list()->append(a_, b_, x_, y_, x_, y_);

    ASSERT( l1->eval()->str() == "List(Scalar(2), Scalar(4), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5), Vector(7, 7, 7, 7, 7, 7, 7, 7, 7, 7), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5), Vector(7, 7, 7, 7, 7, 7, 7, 7, 7, 7))" );
    ASSERT( l1->as<List>()->size() == 6 );

    ExpPtr l2 = maths::list( a_, b_, a_, b_, a_ );

    ASSERT( l2->eval()->str() == "List(Scalar(2), Scalar(4), Scalar(2), Scalar(4), Scalar(2))" );
    ASSERT( l2->as<List>()->size() == 5 );

    ExpPtr c3 = maths::count( maths::list( a_, b_, a_, b_, a_ ) );

    ASSERT( c3->eval()->str() == "Scalar(5)" );
    ASSERT( Scalar::extract(c3->eval()) == 5 );
}

void TestExp::test_fmap()
{
    // simple fmap

    ExpPtr f0 =  fmap( neg(), maths::list( a_ , b_, a_, b_ ) );

    ASSERT( f0->str() == "FMap(Neg(?), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );

    ASSERT( f0->eval()->str() == "List(Scalar(-2), Scalar(-4), Scalar(-2), Scalar(-4))" );

    // fmap with different types

    ExpPtr f1 =  fmap( neg(), maths::list( a_ + b_, x_ ) );

//    std::cout << f1->str() << std::endl;
//    std::cout << f1->eval()->str() << std::endl;

    ASSERT( f1->str() == "FMap(Neg(?), List(Add(Scalar(2), Scalar(4)), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5)))" );

    ASSERT( f1->eval()->str() == "List(Scalar(-6), Vector(-5, -5, -5, -5, -5, -5, -5, -5, -5, -5))" );

}

void TestExp::test_reduce()
{
    // simple reduce

    ExpPtr f0 =  reduce( maths::add(), maths::list( a_ , b_, a_, b_ ) );

    ASSERT( f0->str() == "Reduce(Add(?, ?), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    ASSERT( f0->eval()->str() == "Scalar(12)" );

    // reduce with different types

    ExpPtr f1 =  reduce( maths::prod(), maths::list( a_, x_, x_ ) );

    ASSERT( f1->str() == "Reduce(Prod(?, ?), List(Scalar(2), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5)))" );
    ASSERT( f1->eval()->str() == "Vector(50, 50, 50, 50, 50, 50, 50, 50, 50, 50)" );

}

void TestExp::test_zipwith()
{
    // simple reduce

    ExpPtr f0 =  zipWith( maths::add(), maths::list( a_ , b_, a_, b_ ), maths::list( a_ , b_, a_, b_ ) );

    std::cout << f0->str() << std::endl;
    std::cout << f0->eval()->str() << std::endl;

    ASSERT( f0->eval()->str() == fmap( maths::prod(scalar(2.)), maths::list( a_ , b_, a_, b_ ) )->eval()->str() );

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

