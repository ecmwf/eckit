/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_xpr

#include "ecbuild/boost_test_framework.h"

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

struct ExpFixture {

    ExpFixture() : a_( xpr::scalar( 2. ) ),
                   b_( xpr::scalar( 4. ) ),
                   x_( xpr::vector( 10, 5. ) ),
                   y_( xpr::vector( 10, 7. ) ) {}

    ExpPtr a_;
    ExpPtr b_;
    ExpPtr x_;
    ExpPtr y_;
};

BOOST_FIXTURE_TEST_SUITE( test_eckit_xpr_expr, ExpFixture )

BOOST_AUTO_TEST_CASE( test_optimise_scalars )
{
    ExpPtr c = xpr::add(a_,b_); // scalar-scalar

    ExpPtr e = xpr::add( prod( c , x_ ) , prod( b_, y_ ));

    BOOST_CHECK( e->returnSignature() == Vector::sig() );

    // signature before optimising
    BOOST_CHECK( e->signature() == "Add(Prod(Add(s,s),v),Prod(s,v))" );

    // eval() first calls optimise internally
    BOOST_CHECK( e->eval()->str() == "Vector(58, 58, 58, 58, 58, 58, 58, 58, 58, 58)" );

    ExpPtr opt = e->optimise(true);

    // signature after reducing
    BOOST_CHECK( opt->signature() == "Linear(s,v,s,v)" );
}

BOOST_AUTO_TEST_CASE( test_optimise_recursive_scalars )
{
    ExpPtr c1 = xpr::add(a_,b_);
    ExpPtr c2 = xpr::add(c1,c1);
    ExpPtr c3 = xpr::add(c2,c2);
    ExpPtr c4 = xpr::add(c3,c3);
    ExpPtr e  = xpr::add(c4,c4);

    BOOST_CHECK( e->returnSignature() == Scalar::sig() );

    // signature before reducing
    BOOST_CHECK( e->signature() == "Add(Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))),Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))))" );

    // got fully optimised to a scalar
    BOOST_CHECK( e->optimise(true)->signature() == Scalar::sig() );

    // correct reduction
    BOOST_CHECK( e->eval()->str() == "Scalar(96)" );
}

BOOST_AUTO_TEST_CASE( test_optimise_prodadd )
{
    ExpPtr e0 = xpr::prod( a_, xpr::add(y_, x_ ) );
    BOOST_CHECK( e0->optimise(true)->signature() == "ProdAdd(s,v,v)" );
    BOOST_CHECK( e0->eval()->str() == "Vector(24, 24, 24, 24, 24, 24, 24, 24, 24, 24)" );

    ExpPtr e1 = xpr::prod( x_, xpr::add(y_, x_ ) );
    BOOST_CHECK( e1->optimise(true)->signature() == "ProdAdd(v,v,v)" );
    BOOST_CHECK( e1->eval()->str() == "Vector(60, 60, 60, 60, 60, 60, 60, 60, 60, 60)" );

    // involves also reducing the scalar-scalar
    ExpPtr e2 = xpr::prod( xpr::prod(a_,b_), xpr::add(y_, x_ ) );
    BOOST_CHECK( e2->optimise(true)->signature() == "ProdAdd(s,v,v)" );
    BOOST_CHECK( e2->eval()->str() == "Vector(96, 96, 96, 96, 96, 96, 96, 96, 96, 96)" );
}

BOOST_AUTO_TEST_CASE( test_list )
{
    ExpPtr l0 =  xpr::list( y_ , x_ );

    BOOST_CHECK( l0->eval()->str() == "List(Vector(7, 7, 7, 7, 7, 7, 7, 7, 7, 7), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5))" );
    BOOST_CHECK( l0->arity() == 2 );

    ExpPtr l1 = xpr::list(a_, b_, x_, y_, x_, y_);

    BOOST_CHECK( l1->eval()->str() == "List(Scalar(2), Scalar(4), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5), Vector(7, 7, 7, 7, 7, 7, 7, 7, 7, 7), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5), Vector(7, 7, 7, 7, 7, 7, 7, 7, 7, 7))" );
    BOOST_CHECK( l1->arity() == 6 );

    ExpPtr l2 = xpr::list( a_, b_, a_, b_, a_ );

    BOOST_CHECK( l2->eval()->str() == "List(Scalar(2), Scalar(4), Scalar(2), Scalar(4), Scalar(2))" );
    BOOST_CHECK( l2->arity() == 5 );

    ExpPtr c3 = xpr::count( xpr::list( a_, b_, a_, b_, a_ ) );

    BOOST_CHECK( c3->eval()->str() == "Scalar(5)" );
    BOOST_CHECK( c3->eval()->as<Scalar>()->value() == 5 );
}

BOOST_AUTO_TEST_CASE( test_map )
{
    // simple fmap

    ExpPtr f0 =  xpr::map( neg(), xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK( f0->str() == "Map(Neg(?), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );

    BOOST_CHECK( f0->eval()->str() == "List(Scalar(-2), Scalar(-4), Scalar(-2), Scalar(-4))" );

    // fmap with different types

    ExpPtr f1 =  xpr::map( neg(), xpr::list( xpr::add(a_, b_), x_ ) );

    BOOST_TEST_MESSAGE("Map string representation before eval: " << f1->str());
    BOOST_TEST_MESSAGE("Map string representation after eval: " << f1->eval()->str());

    BOOST_CHECK( f1->str() == "Map(Neg(?), List(Add(Scalar(2), Scalar(4)), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5)))" );

    BOOST_CHECK( f1->eval()->str() == "List(Scalar(-6), Vector(-5, -5, -5, -5, -5, -5, -5, -5, -5, -5))" );
}

BOOST_AUTO_TEST_CASE( test_reduce )
{
    // simple reduce

    ExpPtr f0 =  reduce( xpr::add(), xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK( f0->str() == "Reduce(Add(?, ?), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    BOOST_CHECK( f0->eval()->str() == "Scalar(12)" );

    // reduce with different types

    ExpPtr f1 =  reduce( xpr::prod(), xpr::list( a_, x_, x_ ) );

    BOOST_CHECK( f1->str() == "Reduce(Prod(?, ?), List(Scalar(2), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5), Vector(5, 5, 5, 5, 5, 5, 5, 5, 5, 5)))" );
    BOOST_CHECK( f1->eval()->str() == "Vector(50, 50, 50, 50, 50, 50, 50, 50, 50, 50)" );


    // reduce one element list

    ExpPtr f2 =  reduce( xpr::add(), xpr::list( a_ ) );

    BOOST_CHECK(  f2->str() == "Reduce(Add(?, ?), List(Scalar(2)))" );
    BOOST_CHECK(  f2->eval()->str() == "Scalar(2)" );

    // reduce empty element list

    ExpPtr f3 =  reduce( xpr::add(), xpr::list() );

    BOOST_CHECK(  f3->str() == "Reduce(Add(?, ?), List())" );
    BOOST_CHECK(  f3->eval()->str() == "List()" );
}

BOOST_AUTO_TEST_CASE( test_predicates )
{
    // NotEqual

    ExpPtr f0 =  xpr::not_equal( a_ , b_ );

    BOOST_CHECK( f0->str() == "NotEqual(Scalar(2), Scalar(4))" );
    BOOST_CHECK( f0->eval()->str() == "Boolean(true)" );

    // Greater

    ExpPtr f1 =  xpr::greater( a_ , b_ );

    BOOST_CHECK( f1->str() == "Greater(Scalar(2), Scalar(4))" );
    BOOST_CHECK( f1->eval()->str() == "Boolean(false)" );

    // Less

    ExpPtr f2 =  xpr::less( a_ , b_ );

    BOOST_CHECK( f2->str() == "Less(Scalar(2), Scalar(4))" );
    BOOST_CHECK( f2->eval()->str() == "Boolean(true)" );
}

BOOST_AUTO_TEST_CASE( test_filter )
{
    ExpPtr f3 = xpr::filter( xpr::greater( undef(), xpr::scalar(2) ),xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK( f3->str() == "Filter(Greater(?, Scalar(2)), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    BOOST_CHECK( f3->eval()->str() == "List(Scalar(4), Scalar(4))" );
}

BOOST_AUTO_TEST_CASE( test_bind )
{
    ExpPtr pred = xpr::bind<2>( xpr::greater(), xpr::scalar(2) );

    ExpPtr f0 = xpr::filter( pred, xpr::list( a_ , b_, a_, b_ ) );

    BOOST_TEST_MESSAGE("Bind string representation before eval: " << f0->str());
    BOOST_TEST_MESSAGE("Bind string representation after eval: " << f0->eval()->str());

    BOOST_CHECK( f0->str() == "Filter(Bind(Scalar(2), Greater(?, ?), Scalar(2)), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    BOOST_CHECK( f0->eval()->str() == "List(Scalar(4), Scalar(4))" );
}

BOOST_AUTO_TEST_CASE( test_zipwith )
{
    // simple reduce

    ExpPtr f0 =  zipWith( xpr::add(), xpr::list( a_ , b_, a_, b_ ), xpr::list( a_ , b_, a_, b_ ) );

    BOOST_TEST_MESSAGE("zipWith string representation before eval: " << f0->str());
    BOOST_TEST_MESSAGE("zipWith string representation after eval: " << f0->eval()->str());

    BOOST_CHECK( f0->eval()->str() == xpr::map( xpr::prod(scalar(2.)), xpr::list( a_ , b_, a_, b_ ) )->eval()->str() );

}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

} // namespace eckit_test
