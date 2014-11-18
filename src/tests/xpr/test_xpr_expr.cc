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

#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/BinaryPredicate.h"
#include "eckit/xpr/Count.h"
#include "eckit/xpr/Expression.h"
#include "eckit/xpr/Map.h"
#include "eckit/xpr/Filter.h"
#include "eckit/xpr/Reduce.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/UnaryOperator.h"
#include "eckit/xpr/Vector.h"
#include "eckit/xpr/ZipWith.h"
#include "eckit/xpr/Bind.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

/// generic tests for expression engine

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
    void test_map();
    /// Tests reduce expression
    void test_reduce();
    /// Tests zipwith expression
    void test_zipwith();
    /// Tests predicate expressions
    void test_predicates();
    /// Test filter
    void test_filter();
    /// Test bind
    void test_bind();

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
    test_list();
    test_map();
    test_reduce();
    test_zipwith();
    test_predicates();
    test_filter();
    test_bind();

    teardown();
}

void TestExp::setup()
{
    a_ = xpr::scalar( 2. );
    b_ = xpr::scalar( 4. );
    x_ = xpr::vector( 10, 5. );
    y_ = xpr::vector( 10, 7. );
}

void TestExp::teardown()
{
}

void TestExp::test_optimise_scalars()
{
    ExpPtr c = xpr::add(a_,b_); // scalar-scalar

    ExpPtr e = xpr::add( prod( c , x_ ) , prod( b_, y_ ));

    ASSERT( e->returnSignature() == Vector::sig() );

    // signature before optimising
    ASSERT( e->signature() == "Add(Prod(Add(s,s),v),Prod(s,v))" );

    // eval() first calls optimise internally
    ASSERT( e->eval()->str() == "Vector(58, 58, 58, 58, 58, 58, 58, 58, 58, 58)" );

    ExpPtr opt = e->optimise(true);

    // signature after reducing
    ASSERT( opt->signature() == "Linear(s,v,s,v)" );
}

void TestExp::test_optimise_recursive_scalars()
{
    ExpPtr c1 = xpr::add(a_,b_);
    ExpPtr c2 = xpr::add(c1,c1);
    ExpPtr c3 = xpr::add(c2,c2);
    ExpPtr c4 = xpr::add(c3,c3);
    ExpPtr e  = xpr::add(c4,c4);

    ASSERT( e->returnSignature() == Scalar::sig() );

    // signature before reducing
    ASSERT( e->signature() == "Add(Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))),Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))))" );

    // got fully optimised to a scalar
    ASSERT( e->optimise(true)->signature() == Scalar::sig() );

    // correct reduction
    ASSERT( e->eval()->str() == "Scalar(96)" );

}

void TestExp::test_optimise_prodadd()
{
    ExpPtr e0 = xpr::prod( a_, xpr::add(y_, x_ ) );
    ASSERT( e0->optimise(true)->signature() == "ProdAdd(s,v,v)" );
    ASSERT( e0->eval()->str() == "Vector(24, 24, 24, 24, 24, 24, 24, 24, 24, 24)" );

    ExpPtr e1 = xpr::prod( x_, xpr::add(y_, x_ ) );
    ASSERT( e1->optimise(true)->signature() == "ProdAdd(v,v,v)" );
    ASSERT( e1->eval()->str() == "Vector(60, 60, 60, 60, 60, 60, 60, 60, 60, 60)" );

    // involves also reducing the scalar-scalar
    ExpPtr e2 = xpr::prod( xpr::prod(a_,b_), xpr::add(y_, x_ ) );
    ASSERT( e2->optimise(true)->signature() == "ProdAdd(s,v,v)" );
    ASSERT( e2->eval()->str() == "Vector(96, 96, 96, 96, 96, 96, 96, 96, 96, 96)" );
}

void TestExp::test_list()
{
    ExpPtr l0 =  xpr::list( y_ , x_ );

    ASSERT( l0->eval()->str() == "List(Vector(7, 7, 7, 7, 7, 7, 7, 7, 7, 7), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5))" );
    ASSERT( l0->arity() == 2 );

    ExpPtr l1 = xpr::list(a_, b_, x_, y_, x_, y_);

    ASSERT( l1->eval()->str() == "List(Scalar(2), Scalar(4), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5), Vector(7, 7, 7, 7, 7, 7, 7, 7, 7, 7), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5), Vector(7, 7, 7, 7, 7, 7, 7, 7, 7, 7))" );
    ASSERT( l1->arity() == 6 );

    ExpPtr l2 = xpr::list( a_, b_, a_, b_, a_ );

    ASSERT( l2->eval()->str() == "List(Scalar(2), Scalar(4), Scalar(2), Scalar(4), Scalar(2))" );
    ASSERT( l2->arity() == 5 );

    ExpPtr c3 = xpr::count( xpr::list( a_, b_, a_, b_, a_ ) );

    ASSERT( c3->eval()->str() == "Scalar(5)" );
    ASSERT( c3->eval()->as<Scalar>()->value() == 5 );
}

void TestExp::test_map()
{
    // simple fmap

    ExpPtr f0 =  xpr::map( neg(), xpr::list( a_ , b_, a_, b_ ) );

    ASSERT( f0->str() == "Map(Neg(?), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );

    ASSERT( f0->eval()->str() == "List(Scalar(-2), Scalar(-4), Scalar(-2), Scalar(-4))" );

    // fmap with different types

    ExpPtr f1 =  xpr::map( neg(), xpr::list( xpr::add(a_, b_), x_ ) );

//    std::cout << f1->str() << std::endl;
//    std::cout << f1->eval()->str() << std::endl;

    ASSERT( f1->str() == "Map(Neg(?), List(Add(Scalar(2), Scalar(4)), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5)))" );

    ASSERT( f1->eval()->str() == "List(Scalar(-6), Vector(-5, -5, -5, -5, -5, -5, -5, -5, -5, -5))" );

}

void TestExp::test_reduce()
{
    // simple reduce

    ExpPtr f0 =  reduce( xpr::add(), xpr::list( a_ , b_, a_, b_ ) );

    ASSERT( f0->str() == "Reduce(Add(?, ?), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    ASSERT( f0->eval()->str() == "Scalar(12)" );

    // reduce with different types

    ExpPtr f1 =  reduce( xpr::prod(), xpr::list( a_, x_, x_ ) );

    ASSERT( f1->str() == "Reduce(Prod(?, ?), List(Scalar(2), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5)))" );
    ASSERT( f1->eval()->str() == "Vector(50, 50, 50, 50, 50, 50, 50, 50, 50, 50)" );


    // reduce one element list

    ExpPtr f2 =  reduce( xpr::add(), xpr::list( a_ ) );

    ASSERT(  f2->str() == "Reduce(Add(?, ?), List(Scalar(2)))" );
    ASSERT(  f2->eval()->str() == "Scalar(2)" );

    // reduce empty element list

    ExpPtr f3 =  reduce( xpr::add(), xpr::list() );

    ASSERT(  f3->str() == "Reduce(Add(?, ?), List())" );
    ASSERT(  f3->eval()->str() == "List()" );
}

void TestExp::test_predicates()
{
    // NotEqual

    ExpPtr f0 =  xpr::not_equal( a_ , b_ );

    ASSERT( f0->str() == "NotEqual(Scalar(2), Scalar(4))" );
    ASSERT( f0->eval()->str() == "Boolean(true)" );

    // Greater

    ExpPtr f1 =  xpr::greater( a_ , b_ );

    ASSERT( f1->str() == "Greater(Scalar(2), Scalar(4))" );
    ASSERT( f1->eval()->str() == "Boolean(false)" );

    // Less

    ExpPtr f2 =  xpr::less( a_ , b_ );

    ASSERT( f2->str() == "Less(Scalar(2), Scalar(4))" );
    ASSERT( f2->eval()->str() == "Boolean(true)" );
}

void TestExp::test_filter()
{
    ExpPtr f3 = xpr::filter( xpr::greater( undef(), xpr::scalar(2) ),xpr::list( a_ , b_, a_, b_ ) );

    ASSERT( f3->str() == "Filter(Greater(?, Scalar(2)), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    ASSERT( f3->eval()->str() == "List(Scalar(4), Scalar(4))" );
}

void TestExp::test_bind()
{
    ExpPtr pred = xpr::bind<2>( xpr::greater(), xpr::scalar(2) );

    ExpPtr f0 = xpr::filter( pred, xpr::list( a_ , b_, a_, b_ ) );

//        std::cout << f0->str() << std::endl;
//        std::cout << f0->eval()->str() << std::endl;

    ASSERT( f0->str() == "Filter(Bind(Scalar(2), Greater(?, ?), Scalar(2)), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    ASSERT( f0->eval()->str() == "List(Scalar(4), Scalar(4))" );
}

void TestExp::test_zipwith()
{
    // simple reduce

    ExpPtr f0 =  zipWith( xpr::add(), xpr::list( a_ , b_, a_, b_ ), xpr::list( a_ , b_, a_, b_ ) );

//    std::cout << f0->str() << std::endl;
//    std::cout << f0->eval()->str() << std::endl;

    ASSERT( f0->eval()->str() == xpr::map( xpr::prod(scalar(2.)), xpr::list( a_ , b_, a_, b_ ) )->eval()->str() );

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

