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
                   x_( xpr::vector( 3, 5. ) ),
                   y_( xpr::vector( 3, 7. ) ) {}

    ExpPtr a_;
    ExpPtr b_;
    ExpPtr x_;
    ExpPtr y_;
};

BOOST_FIXTURE_TEST_SUITE( test_eckit_xpr_expr, ExpFixture )

BOOST_AUTO_TEST_CASE( test_optimise_scalars )
{
    ExpPtr c = add(a_,b_); // scalar-scalar

    ExpPtr e = add( prod( c , x_ ) , prod( b_, y_ ));

    BOOST_CHECK( e->returnSignature() == Vector::sig() );

    // signature and code representation before optimising
    BOOST_CHECK( e->signature() == "Add(Prod(Add(s,s),v),Prod(s,v))" );
    BOOST_CHECK( e->code() == "xpr::add(xpr::prod(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::vector({5, 5, 5})), xpr::prod(xpr::scalar(4), xpr::vector({7, 7, 7})))" );
    BOOST_CHECK( e->json() == "{\"xpr::add\":[{\"xpr::prod\":[{\"xpr::add\":[2,4]},[5,5,5]]},{\"xpr::prod\":[4,[7,7,7]]}]}" );

    // eval() first calls optimise internally
    BOOST_CHECK_EQUAL( xpr::add(xpr::prod(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::vector({5, 5, 5})), xpr::prod(xpr::scalar(4), xpr::vector({7, 7, 7})))->eval()->str(), "Vector(58, 58, 58)" );
    BOOST_CHECK_EQUAL( e->eval()->str(), "Vector(58, 58, 58)" );
    BOOST_CHECK_EQUAL( e->eval()->code(), "xpr::vector({58, 58, 58})" );
    BOOST_CHECK_EQUAL( e->eval()->json(), "[58,58,58]" );

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

    // signature and code representation before reducing
    BOOST_CHECK( e->signature() == "Add(Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))),Add(Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s))),Add(Add(Add(s,s),Add(s,s)),Add(Add(s,s),Add(s,s)))))" );
    BOOST_CHECK( e->code() == "xpr::add(xpr::add(xpr::add(xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))), xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4)))), xpr::add(xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))), xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))))), xpr::add(xpr::add(xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))), xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4)))), xpr::add(xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))), xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))))))" );
    BOOST_CHECK( e->json() == "{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]},{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]}]},{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]},{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]}]}]},{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]},{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]}]},{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]},{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]}]}]}]}" );

    // got fully optimised to a scalar
    BOOST_CHECK( e->optimise(true)->signature() == Scalar::sig() );

    // correct reduction
    BOOST_CHECK( xpr::add(xpr::add(xpr::add(xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))), xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4)))), xpr::add(xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))), xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))))), xpr::add(xpr::add(xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))), xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4)))), xpr::add(xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))), xpr::add(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::add(xpr::scalar(2), xpr::scalar(4))))))->eval()->str() == "Scalar(96)" );
    BOOST_CHECK( e->eval()->str() == "Scalar(96)" );
    BOOST_CHECK( e->eval()->code() == "xpr::scalar(96)" );
    BOOST_CHECK( e->eval()->json() == "96" );
}

BOOST_AUTO_TEST_CASE( test_optimise_prodadd )
{
    ExpPtr e0 = xpr::prod( a_, xpr::add(y_, x_ ) );
    BOOST_CHECK( e0->optimise(true)->signature() == "ProdAdd(s,v,v)" );
    BOOST_CHECK( e0->eval()->str() == "Vector(24, 24, 24)" );
    BOOST_CHECK( e0->eval()->code() == "xpr::vector({24, 24, 24})" );
    BOOST_CHECK( e0->eval()->json() == "[24,24,24]" );

    ExpPtr e1 = xpr::prod( x_, xpr::add(y_, x_ ) );
    BOOST_CHECK( e1->optimise(true)->signature() == "ProdAdd(v,v,v)" );
    BOOST_CHECK( e1->eval()->str() == "Vector(60, 60, 60)" );
    BOOST_CHECK( e1->eval()->code() == "xpr::vector({60, 60, 60})" );
    BOOST_CHECK( e1->eval()->json() == "[60,60,60]" );

    // involves also reducing the scalar-scalar
    ExpPtr e2 = xpr::prod( xpr::prod(a_,b_), xpr::add(y_, x_ ) );
    BOOST_CHECK( e2->optimise(true)->signature() == "ProdAdd(s,v,v)" );
    BOOST_CHECK( e2->eval()->str() == "Vector(96, 96, 96)" );
    BOOST_CHECK( e2->eval()->code() == "xpr::vector({96, 96, 96})" );
    BOOST_CHECK( e2->eval()->json() == "[96,96,96]" );
}

BOOST_AUTO_TEST_CASE( test_list )
{
    ExpPtr l0 =  xpr::list( y_ , x_ );

    BOOST_CHECK( l0->eval()->str() == "List(Vector(7, 7, 7), Vector(5, 5, 5))" );
    BOOST_CHECK( l0->eval()->code() == "xpr::list(xpr::vector({7, 7, 7}), xpr::vector({5, 5, 5}))" );
    BOOST_CHECK( l0->eval()->json() == "{\"xpr::list\":[[7,7,7],[5,5,5]]}" );
    BOOST_CHECK( l0->arity() == 2 );

    ExpPtr l1 = xpr::list(a_, b_, x_, y_, x_, y_);

    BOOST_CHECK( l1->eval()->str() == "List(Scalar(2), Scalar(4), Vector(5, 5, 5), Vector(7, 7, 7), Vector(5, 5, 5), Vector(7, 7, 7))" );
    BOOST_CHECK( l1->eval()->code() == "xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::vector({5, 5, 5}), xpr::vector({7, 7, 7}), xpr::vector({5, 5, 5}), xpr::vector({7, 7, 7}))" );
    BOOST_CHECK( l1->eval()->json() == "{\"xpr::list\":[2,4,[5,5,5],[7,7,7],[5,5,5],[7,7,7]]}" );
    BOOST_CHECK( l1->arity() == 6 );

    ExpPtr l2 = xpr::list( a_, b_, a_, b_, a_ );

    BOOST_CHECK( l2->eval()->str() == "List(Scalar(2), Scalar(4), Scalar(2), Scalar(4), Scalar(2))" );
    BOOST_CHECK( l2->eval()->code() == "xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::scalar(2), xpr::scalar(4), xpr::scalar(2))" );
    BOOST_CHECK( l2->eval()->json() == "{\"xpr::list\":[2,4,2,4,2]}" );
    BOOST_CHECK( l2->arity() == 5 );

    ExpPtr c3 = xpr::count( xpr::list( a_, b_, a_, b_, a_ ) );

    BOOST_CHECK( c3->eval()->str() == "Scalar(5)" );
    BOOST_CHECK( c3->eval()->code() == "xpr::scalar(5)" );
    BOOST_CHECK( c3->eval()->json() == "5" );
    BOOST_CHECK( c3->eval()->as<Scalar>()->value() == 5 );
}

BOOST_AUTO_TEST_CASE( test_map )
{
    // simple fmap

    ExpPtr f0 =  xpr::map( neg(), xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK( f0->str() == "Map(Neg(?), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    BOOST_CHECK( f0->code() == "xpr::map(xpr::neg(xpr::undef()), xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::scalar(2), xpr::scalar(4)))" );
    BOOST_CHECK( f0->json() == "{\"xpr::map\":[{\"xpr::neg\":\"xpr::undef\"},{\"xpr::list\":[2,4,2,4]}]}" );

    BOOST_CHECK( xpr::map(xpr::neg(xpr::undef()), xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::scalar(2), xpr::scalar(4)))->eval()->str() == "List(Scalar(-2), Scalar(-4), Scalar(-2), Scalar(-4))" );
    BOOST_CHECK( f0->eval()->str() == "List(Scalar(-2), Scalar(-4), Scalar(-2), Scalar(-4))" );
    BOOST_CHECK( f0->eval()->code() == "xpr::list(xpr::scalar(-2), xpr::scalar(-4), xpr::scalar(-2), xpr::scalar(-4))" );
    BOOST_CHECK( f0->eval()->json() == "{\"xpr::list\":[-2,-4,-2,-4]}" );

    // fmap with different types

    ExpPtr f1 =  xpr::map( neg(), xpr::list( xpr::add(a_, b_), x_ ) );

    BOOST_TEST_MESSAGE("Map string representation before eval: " << f1->str());
    BOOST_TEST_MESSAGE("Map string representation after eval: " << f1->eval()->str());

    BOOST_CHECK( f1->str() == "Map(Neg(?), List(Add(Scalar(2), Scalar(4)), Vector(5, 5, 5)))" );
    BOOST_CHECK( f1->code() == "xpr::map(xpr::neg(xpr::undef()), xpr::list(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::vector({5, 5, 5})))" );
    BOOST_CHECK( f1->json() == "{\"xpr::map\":[{\"xpr::neg\":\"xpr::undef\"},{\"xpr::list\":[{\"xpr::add\":[2,4]},[5,5,5]]}]}" );

    BOOST_CHECK( xpr::map(xpr::neg(xpr::undef()), xpr::list(xpr::add(xpr::scalar(2), xpr::scalar(4)), xpr::vector({5, 5, 5})))->eval()->str() == "List(Scalar(-6), Vector(-5, -5, -5))" );
    BOOST_CHECK( f1->eval()->code() == "xpr::list(xpr::scalar(-6), xpr::vector({-5, -5, -5}))" );
    BOOST_CHECK( f1->eval()->json() == "{\"xpr::list\":[-6,[-5,-5,-5]]}" );
}

BOOST_AUTO_TEST_CASE( test_reduce )
{
    // simple reduce

    ExpPtr f0 =  reduce( xpr::add(), xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK( f0->str() == "Reduce(Add(?, ?), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    BOOST_CHECK( f0->code() == "xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::scalar(2), xpr::scalar(4)))" );
    BOOST_CHECK( f0->json() == "{\"xpr::reduce\":[{\"xpr::add\":[\"xpr::undef\",\"xpr::undef\"]},{\"xpr::list\":[2,4,2,4]}]}" );
    BOOST_CHECK( xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::scalar(2), xpr::scalar(4)))->eval()->str() == "Scalar(12)" );
    BOOST_CHECK( f0->eval()->str() == "Scalar(12)" );
    BOOST_CHECK( f0->eval()->code() == "xpr::scalar(12)" );
    BOOST_CHECK( f0->eval()->json() == "12" );

    // reduce with different types

    ExpPtr f1 =  reduce( xpr::prod(), xpr::list( a_, x_, x_ ) );

    BOOST_CHECK( f1->str() == "Reduce(Prod(?, ?), List(Scalar(2), Vector(5, 5, 5), Vector(5, 5, 5)))" );
    BOOST_CHECK( f1->code() == "xpr::reduce(xpr::prod(xpr::undef(), xpr::undef()), xpr::list(xpr::scalar(2), xpr::vector({5, 5, 5}), xpr::vector({5, 5, 5})))" );
    BOOST_CHECK( f1->json() == "{\"xpr::reduce\":[{\"xpr::prod\":[\"xpr::undef\",\"xpr::undef\"]},{\"xpr::list\":[2,[5,5,5],[5,5,5]]}]}" );
    BOOST_CHECK( xpr::reduce(xpr::prod(xpr::undef(), xpr::undef()), xpr::list(xpr::scalar(2), xpr::vector({5, 5, 5}), xpr::vector({5, 5, 5})))->eval()->str() == "Vector(50, 50, 50)" );
    BOOST_CHECK( f1->eval()->str() == "Vector(50, 50, 50)" );
    BOOST_CHECK( f1->eval()->code() == "xpr::vector({50, 50, 50})" );
    BOOST_CHECK( f1->eval()->json() == "[50,50,50]" );


    // reduce one element list

    ExpPtr f2 = reduce( xpr::add(), xpr::list( a_ ) );

    BOOST_CHECK( f2->str() == "Reduce(Add(?, ?), List(Scalar(2)))" );
    BOOST_CHECK( f2->code() == "xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list(xpr::scalar(2)))" );
    BOOST_CHECK( f2->json() == "{\"xpr::reduce\":[{\"xpr::add\":[\"xpr::undef\",\"xpr::undef\"]},{\"xpr::list\":[2]}]}" );
    BOOST_CHECK( xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list(xpr::scalar(2)))->eval()->str() == "Scalar(2)" );
    BOOST_CHECK( f2->eval()->str() == "Scalar(2)" );
    BOOST_CHECK( f2->eval()->code() == "xpr::scalar(2)" );
    BOOST_CHECK( f2->eval()->json() == "2" );

    // reduce empty element list

    ExpPtr f3 =  reduce( xpr::add(), xpr::list() );

    BOOST_CHECK( f3->str() == "Reduce(Add(?, ?), List())" );
    BOOST_CHECK( f3->code() == "xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list())" );
    BOOST_CHECK( f3->json() == "{\"xpr::reduce\":[{\"xpr::add\":[\"xpr::undef\",\"xpr::undef\"]},{\"xpr::list\":[]}]}" );
    BOOST_CHECK( xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list())->eval()->str() == "List()" );
    BOOST_CHECK( f3->eval()->str() == "List()" );
    BOOST_CHECK( f3->eval()->code() == "xpr::list()" );
    BOOST_CHECK( f3->eval()->json() == "{\"xpr::list\":[]}" );
}

BOOST_AUTO_TEST_CASE( test_predicates )
{
    // NotEqual

    ExpPtr f0 =  xpr::not_equal( a_ , b_ );

    BOOST_CHECK( f0->str() == "NotEqual(Scalar(2), Scalar(4))" );
    BOOST_CHECK( f0->code() == "xpr::not_equal(xpr::scalar(2), xpr::scalar(4))" );
    BOOST_CHECK( f0->json() == "{\"xpr::not_equal\":[2,4]}" );
    BOOST_CHECK( xpr::not_equal(xpr::scalar(2), xpr::scalar(4))->eval()->str() == "Boolean(true)" );
    BOOST_CHECK( f0->eval()->str() == "Boolean(true)" );
    BOOST_CHECK( f0->eval()->code() == "xpr::boolean(true)" );
    BOOST_CHECK( f0->eval()->json() == "true" );

    // Greater

    ExpPtr f1 =  xpr::greater( a_ , b_ );

    BOOST_CHECK( f1->str() == "Greater(Scalar(2), Scalar(4))" );
    BOOST_CHECK( f1->code() == "xpr::greater(xpr::scalar(2), xpr::scalar(4))" );
    BOOST_CHECK( f1->json() == "{\"xpr::greater\":[2,4]}" );
    BOOST_CHECK( xpr::greater(xpr::scalar(2), xpr::scalar(4))->eval()->str() == "Boolean(false)" );
    BOOST_CHECK( f1->eval()->str() == "Boolean(false)" );
    BOOST_CHECK( f1->eval()->code() == "xpr::boolean(false)" );
    BOOST_CHECK( f1->eval()->json() == "false" );

    // Less

    ExpPtr f2 =  xpr::less( a_ , b_ );

    BOOST_CHECK( f2->str() == "Less(Scalar(2), Scalar(4))" );
    BOOST_CHECK( f2->code() == "xpr::less(xpr::scalar(2), xpr::scalar(4))" );
    BOOST_CHECK( f2->json() == "{\"xpr::less\":[2,4]}" );
    BOOST_CHECK( xpr::less(xpr::scalar(2), xpr::scalar(4))->eval()->str() == "Boolean(true)" );
    BOOST_CHECK( f2->eval()->str() == "Boolean(true)" );
    BOOST_CHECK( f2->eval()->code() == "xpr::boolean(true)" );
    BOOST_CHECK( f2->eval()->json() == "true" );
}

BOOST_AUTO_TEST_CASE( test_filter )
{
    ExpPtr f3 = xpr::filter( xpr::greater( undef(), xpr::scalar(2) ),xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK( f3->str() == "Filter(Greater(?, Scalar(2)), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    BOOST_CHECK( f3->code() == "xpr::filter(xpr::greater(xpr::undef(), xpr::scalar(2)), xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::scalar(2), xpr::scalar(4)))" );
    BOOST_CHECK( f3->json() == "{\"xpr::filter\":[{\"xpr::greater\":[\"xpr::undef\",2]},{\"xpr::list\":[2,4,2,4]}]}" );
    BOOST_CHECK( xpr::filter(xpr::greater(xpr::undef(), xpr::scalar(2)), xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::scalar(2), xpr::scalar(4)))->eval()->str() == "List(Scalar(4), Scalar(4))" );
    BOOST_CHECK( f3->eval()->str() == "List(Scalar(4), Scalar(4))" );
    BOOST_CHECK( f3->eval()->code() == "xpr::list(xpr::scalar(4), xpr::scalar(4))" );
    BOOST_CHECK( f3->eval()->json() == "{\"xpr::list\":[4,4]}" );
}

BOOST_AUTO_TEST_CASE( test_bind )
{
    ExpPtr pred = xpr::bind<2>( xpr::greater(), xpr::scalar(2) );

    ExpPtr f0 = xpr::filter( pred, xpr::list( a_ , b_, a_, b_ ) );

    BOOST_TEST_MESSAGE("Bind string representation before eval: " << f0->str());
    BOOST_TEST_MESSAGE("Bind string representation after eval: " << f0->eval()->str());

    BOOST_CHECK( f0->str() == "Filter(Bind(Scalar(2), Greater(?, ?), Scalar(2)), List(Scalar(2), Scalar(4), Scalar(2), Scalar(4)))" );
    BOOST_CHECK( f0->code() == "xpr::filter(xpr::bind<2>(xpr::greater(xpr::undef(), xpr::undef()), xpr::scalar(2)), xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::scalar(2), xpr::scalar(4)))" );
    BOOST_CHECK( f0->json() == "{\"xpr::filter\":[{\"xpr::bind\":[2,{\"xpr::greater\":[\"xpr::undef\",\"xpr::undef\"]},2]},{\"xpr::list\":[2,4,2,4]}]}" );
    BOOST_CHECK( xpr::filter(xpr::bind<2>(xpr::greater(xpr::undef(), xpr::undef()), xpr::scalar(2)), xpr::list(xpr::scalar(2), xpr::scalar(4), xpr::scalar(2), xpr::scalar(4)))->eval()->str() == "List(Scalar(4), Scalar(4))" );
    BOOST_CHECK( f0->eval()->str() == "List(Scalar(4), Scalar(4))" );
    BOOST_CHECK( f0->eval()->code() == "xpr::list(xpr::scalar(4), xpr::scalar(4))" );
    BOOST_CHECK( f0->eval()->json() == "{\"xpr::list\":[4,4]}" );
}

BOOST_AUTO_TEST_CASE( test_zipwith )
{
    // simple reduce

    ExpPtr f0 =  zipWith( xpr::add(), xpr::list( a_ , b_, a_, b_ ), xpr::list( a_ , b_, a_, b_ ) );

    BOOST_TEST_MESSAGE("zipWith string representation before eval: " << f0->str());
    BOOST_TEST_MESSAGE("zipWith string representation after eval: " << f0->eval()->str());

    BOOST_CHECK( f0->eval()->str() == xpr::map( xpr::prod(scalar(2.)), xpr::list( a_ , b_, a_, b_ ) )->eval()->str() );
    BOOST_CHECK( f0->eval()->code() == "xpr::list(xpr::scalar(4), xpr::scalar(8), xpr::scalar(4), xpr::scalar(8))" );
    BOOST_CHECK( f0->eval()->json() == "{\"xpr::list\":[4,8,4,8]}" );

}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

} // namespace eckit_test
