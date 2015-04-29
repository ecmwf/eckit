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
#include "eckit/xpr/IfElse.h"
#include "eckit/xpr/Reduce.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/String.h"
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

    ExpFixture() : a_( xpr::real( 2. ) ),
                   b_( xpr::real( 4. ) ),
                   x_( xpr::vector( 3, 5. ) ),
                   y_( xpr::vector( 3, 7. ) ) {}

    ExpPtr a_;
    ExpPtr b_;
    ExpPtr x_;
    ExpPtr y_;
};

BOOST_FIXTURE_TEST_SUITE( test_eckit_xpr_expr, ExpFixture )

BOOST_AUTO_TEST_CASE( test_optimise_reals )
{
    ExpPtr c = add(a_,b_); // real-real

    ExpPtr e = add( prod( c , x_ ) , prod( b_, y_ ));

    // signature and code representation before optimising
    BOOST_CHECK_EQUAL( e->signature() , "xpr::Add(xpr::Prod(xpr::Add(r,r),v),xpr::Prod(r,v))" );
    BOOST_CHECK_EQUAL( e->code() , "xpr::add(xpr::prod(xpr::add(xpr::real(2), xpr::real(4)), xpr::vector({5, 5, 5})), xpr::prod(xpr::real(4), xpr::vector({7, 7, 7})))" );
    BOOST_CHECK_EQUAL( e->json() , "{\"xpr::add\":[{\"xpr::prod\":[{\"xpr::add\":[2,4]},[5,5,5]]},{\"xpr::prod\":[4,[7,7,7]]}]}" );

    // eval() first calls optimise internally
    BOOST_CHECK_EQUAL( xpr::add(xpr::prod(xpr::add(xpr::real(2), xpr::real(4)), xpr::vector({5, 5, 5})), xpr::prod(xpr::real(4), xpr::vector({7, 7, 7})))->eval()->str(), "xpr::Vector(58, 58, 58)" );
    BOOST_CHECK_EQUAL( e->eval()->str(), "xpr::Vector(58, 58, 58)" );
    BOOST_CHECK_EQUAL( e->eval()->code(), "xpr::vector({58, 58, 58})" );
    BOOST_CHECK_EQUAL( e->eval()->json(), "[58,58,58]" );

    ExpPtr opt = e->optimise(true);

    // signature after reducing
    BOOST_CHECK_EQUAL( opt->signature() , "xpr::Linear(r,v,r,v)" );
}

BOOST_AUTO_TEST_CASE( test_optimise_recursive_reals )
{
    ExpPtr c1 = xpr::add(a_,b_);
    ExpPtr c2 = xpr::add(c1,c1);
    ExpPtr c3 = xpr::add(c2,c2);
    ExpPtr c4 = xpr::add(c3,c3);
    ExpPtr e  = xpr::add(c4,c4);

    // signature and code representation before reducing
    BOOST_CHECK_EQUAL( e->signature() , "xpr::Add(xpr::Add(xpr::Add(xpr::Add(xpr::Add(r,r),xpr::Add(r,r)),xpr::Add(xpr::Add(r,r),xpr::Add(r,r))),xpr::Add(xpr::Add(xpr::Add(r,r),xpr::Add(r,r)),xpr::Add(xpr::Add(r,r),xpr::Add(r,r)))),xpr::Add(xpr::Add(xpr::Add(xpr::Add(r,r),xpr::Add(r,r)),xpr::Add(xpr::Add(r,r),xpr::Add(r,r))),xpr::Add(xpr::Add(xpr::Add(r,r),xpr::Add(r,r)),xpr::Add(xpr::Add(r,r),xpr::Add(r,r)))))" );
    BOOST_CHECK_EQUAL( e->code() , "xpr::add(xpr::add(xpr::add(xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))), xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4)))), xpr::add(xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))), xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))))), xpr::add(xpr::add(xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))), xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4)))), xpr::add(xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))), xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))))))" );
    BOOST_CHECK_EQUAL( e->json() , "{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]},{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]}]},{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]},{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]}]}]},{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]},{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]}]},{\"xpr::add\":[{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]},{\"xpr::add\":[{\"xpr::add\":[2,4]},{\"xpr::add\":[2,4]}]}]}]}]}" );

    // got fully optimised to a real
    BOOST_CHECK_EQUAL( e->optimise(true)->signature() , Real::sig() );

    // correct reduction
    BOOST_CHECK_EQUAL( xpr::add(xpr::add(xpr::add(xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))), xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4)))), xpr::add(xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))), xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))))), xpr::add(xpr::add(xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))), xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4)))), xpr::add(xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))), xpr::add(xpr::add(xpr::real(2), xpr::real(4)), xpr::add(xpr::real(2), xpr::real(4))))))->eval()->str() , "xpr::Real(96)" );
    BOOST_CHECK_EQUAL( e->eval()->str() , "xpr::Real(96)" );
    BOOST_CHECK_EQUAL( e->eval()->code() , "xpr::real(96)" );
    BOOST_CHECK_EQUAL( e->eval()->json() , "96" );
}

BOOST_AUTO_TEST_CASE( test_optimise_prodadd )
{
    ExpPtr e0 = xpr::prod( a_, xpr::add(y_, x_ ) );
    BOOST_CHECK_EQUAL( e0->optimise(true)->signature() , "xpr::ProdAdd(r,v,v)" );
    BOOST_CHECK_EQUAL( e0->eval()->str() , "xpr::Vector(24, 24, 24)" );
    BOOST_CHECK_EQUAL( e0->eval()->code() , "xpr::vector({24, 24, 24})" );
    BOOST_CHECK_EQUAL( e0->eval()->json() , "[24,24,24]" );

    ExpPtr e1 = xpr::prod( x_, xpr::add(y_, x_ ) );
    BOOST_CHECK_EQUAL( e1->optimise(true)->signature() , "xpr::ProdAdd(v,v,v)" );
    BOOST_CHECK_EQUAL( e1->eval()->str() , "xpr::Vector(60, 60, 60)" );
    BOOST_CHECK_EQUAL( e1->eval()->code() , "xpr::vector({60, 60, 60})" );
    BOOST_CHECK_EQUAL( e1->eval()->json() , "[60,60,60]" );

    // involves also reducing the real-real
    ExpPtr e2 = xpr::prod( xpr::prod(a_,b_), xpr::add(y_, x_ ) );
    BOOST_CHECK_EQUAL( e2->optimise(true)->signature() , "xpr::ProdAdd(r,v,v)" );
    BOOST_CHECK_EQUAL( e2->eval()->str() , "xpr::Vector(96, 96, 96)" );
    BOOST_CHECK_EQUAL( e2->eval()->code() , "xpr::vector({96, 96, 96})" );
    BOOST_CHECK_EQUAL( e2->eval()->json() , "[96,96,96]" );
}

BOOST_AUTO_TEST_CASE( test_if )
{
    ExpPtr i = xpr::ifelse( xpr::boolean( xpr::count( xpr::list( 7, 8, 9 ) ) ),
                            xpr::string("I'm True"), xpr::string("I'm False") );

    BOOST_CHECK_EQUAL( i->eval()->str() , "xpr::String(\"I'm True\")" );
    BOOST_CHECK_EQUAL( i->eval()->code() , "xpr::string(\"I'm True\")" );
    BOOST_CHECK_EQUAL( i->eval()->json() , "\"I'm True\"" );
}

BOOST_AUTO_TEST_CASE( test_list )
{
    ExpPtr l0 =  xpr::list( y_ , x_ );

    BOOST_CHECK_EQUAL( l0->eval()->str() , "xpr::List(xpr::Vector(7, 7, 7), xpr::Vector(5, 5, 5))" );
    BOOST_CHECK_EQUAL( l0->eval()->code() , "xpr::list(xpr::vector({7, 7, 7}), xpr::vector({5, 5, 5}))" );
    BOOST_CHECK_EQUAL( l0->eval()->json() , "{\"xpr::list\":[[7,7,7],[5,5,5]]}" );
    BOOST_CHECK_EQUAL( l0->arity() , 2 );

    ExpPtr l1 = xpr::list(a_, b_, x_, y_, x_, y_);

    BOOST_CHECK_EQUAL( l1->eval()->str() , "xpr::List(xpr::Real(2), xpr::Real(4), xpr::Vector(5, 5, 5), xpr::Vector(7, 7, 7), xpr::Vector(5, 5, 5), xpr::Vector(7, 7, 7))" );
    BOOST_CHECK_EQUAL( l1->eval()->code() , "xpr::list(xpr::real(2), xpr::real(4), xpr::vector({5, 5, 5}), xpr::vector({7, 7, 7}), xpr::vector({5, 5, 5}), xpr::vector({7, 7, 7}))" );
    BOOST_CHECK_EQUAL( l1->eval()->json() , "{\"xpr::list\":[2,4,[5,5,5],[7,7,7],[5,5,5],[7,7,7]]}" );
    BOOST_CHECK_EQUAL( l1->arity() , 6 );

    ExpPtr l2 = xpr::list( a_, b_, a_, b_, a_ );

    BOOST_CHECK_EQUAL( l2->eval()->str() , "xpr::List(xpr::Real(2), xpr::Real(4), xpr::Real(2), xpr::Real(4), xpr::Real(2))" );
    BOOST_CHECK_EQUAL( l2->eval()->code() , "xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::real(4), xpr::real(2))" );
    BOOST_CHECK_EQUAL( l2->eval()->json() , "{\"xpr::list\":[2,4,2,4,2]}" );
    BOOST_CHECK_EQUAL( l2->arity() , 5 );

    ExpPtr l3 = xpr::list( a_, b_, 2., 4 );

    BOOST_CHECK_EQUAL( l3->eval()->str() , "xpr::List(xpr::Real(2), xpr::Real(4), xpr::Real(2), xpr::Integer(4))" );
    BOOST_CHECK_EQUAL( l3->eval()->code() , "xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::integer(4))" );
    BOOST_CHECK_EQUAL( l3->eval()->json() , "{\"xpr::list\":[2,4,2,4]}" );
    BOOST_CHECK_EQUAL( l3->arity() , 4 );

    ExpPtr c4 = xpr::count( xpr::list( a_, b_, a_, b_, a_ ) );

    BOOST_CHECK_EQUAL( c4->eval()->str() , "xpr::Integer(5)" );
    BOOST_CHECK_EQUAL( c4->eval()->code() , "xpr::integer(5)" );
    BOOST_CHECK_EQUAL( c4->eval()->json() , "5" );
    BOOST_CHECK_EQUAL( c4->eval()->as<Integer>()->value() , 5 );
}

BOOST_AUTO_TEST_CASE( test_map )
{
    // simple fmap

    ExpPtr f0 =  xpr::map( neg(), xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK_EQUAL( f0->str() , "xpr::Map(xpr::Neg(?), xpr::List(xpr::Real(2), xpr::Real(4), xpr::Real(2), xpr::Real(4)))" );
    BOOST_CHECK_EQUAL( f0->code() , "xpr::map(xpr::neg(xpr::undef()), xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::real(4)))" );
    BOOST_CHECK_EQUAL( f0->json() , "{\"xpr::map\":[{\"xpr::neg\":\"xpr::undef\"},{\"xpr::list\":[2,4,2,4]}]}" );

    BOOST_CHECK_EQUAL( xpr::map(xpr::neg(xpr::undef()), xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::real(4)))->eval()->str() , "xpr::List(xpr::Real(-2), xpr::Real(-4), xpr::Real(-2), xpr::Real(-4))" );
    BOOST_CHECK_EQUAL( f0->eval()->str() , "xpr::List(xpr::Real(-2), xpr::Real(-4), xpr::Real(-2), xpr::Real(-4))" );
    BOOST_CHECK_EQUAL( f0->eval()->code() , "xpr::list(xpr::real(-2), xpr::real(-4), xpr::real(-2), xpr::real(-4))" );
    BOOST_CHECK_EQUAL( f0->eval()->json() , "{\"xpr::list\":[-2,-4,-2,-4]}" );

    // fmap with different types

    ExpPtr f1 =  xpr::map( neg(), xpr::list( xpr::add(a_, b_), x_ ) );

    BOOST_CHECK_EQUAL( f1->str() , "xpr::Map(xpr::Neg(?), xpr::List(xpr::Add(xpr::Real(2), xpr::Real(4)), xpr::Vector(5, 5, 5)))" );
    BOOST_CHECK_EQUAL( f1->code() , "xpr::map(xpr::neg(xpr::undef()), xpr::list(xpr::add(xpr::real(2), xpr::real(4)), xpr::vector({5, 5, 5})))" );
    BOOST_CHECK_EQUAL( f1->json() , "{\"xpr::map\":[{\"xpr::neg\":\"xpr::undef\"},{\"xpr::list\":[{\"xpr::add\":[2,4]},[5,5,5]]}]}" );

    BOOST_CHECK_EQUAL( xpr::map(xpr::neg(xpr::undef()), xpr::list(xpr::add(xpr::real(2), xpr::real(4)), xpr::vector({5, 5, 5})))->eval()->str() , "xpr::List(xpr::Real(-6), xpr::Vector(-5, -5, -5))" );
    BOOST_CHECK_EQUAL( f1->eval()->code() , "xpr::list(xpr::real(-6), xpr::vector({-5, -5, -5}))" );
    BOOST_CHECK_EQUAL( f1->eval()->json() , "{\"xpr::list\":[-6,[-5,-5,-5]]}" );
}

BOOST_AUTO_TEST_CASE( test_reduce )
{
    // simple reduce

    ExpPtr f0 =  reduce( xpr::add(), xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK_EQUAL( f0->str() , "xpr::Reduce(xpr::Add(?, ?), xpr::List(xpr::Real(2), xpr::Real(4), xpr::Real(2), xpr::Real(4)))" );
    BOOST_CHECK_EQUAL( f0->code() , "xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::real(4)))" );
    BOOST_CHECK_EQUAL( f0->json() , "{\"xpr::reduce\":[{\"xpr::add\":[\"xpr::undef\",\"xpr::undef\"]},{\"xpr::list\":[2,4,2,4]}]}" );
    BOOST_CHECK_EQUAL( xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::real(4)))->eval()->str() , "xpr::Real(12)" );
    BOOST_CHECK_EQUAL( f0->eval()->str() , "xpr::Real(12)" );
    BOOST_CHECK_EQUAL( f0->eval()->code() , "xpr::real(12)" );
    BOOST_CHECK_EQUAL( f0->eval()->json() , "12" );

    // reduce with different types

    ExpPtr f1 =  reduce( xpr::prod(), xpr::list( a_, x_, x_ ) );

    BOOST_CHECK_EQUAL( f1->str() , "xpr::Reduce(xpr::Prod(?, ?), xpr::List(xpr::Real(2), xpr::Vector(5, 5, 5), xpr::Vector(5, 5, 5)))" );
    BOOST_CHECK_EQUAL( f1->code() , "xpr::reduce(xpr::prod(xpr::undef(), xpr::undef()), xpr::list(xpr::real(2), xpr::vector({5, 5, 5}), xpr::vector({5, 5, 5})))" );
    BOOST_CHECK_EQUAL( f1->json() , "{\"xpr::reduce\":[{\"xpr::prod\":[\"xpr::undef\",\"xpr::undef\"]},{\"xpr::list\":[2,[5,5,5],[5,5,5]]}]}" );
    BOOST_CHECK_EQUAL( xpr::reduce(xpr::prod(xpr::undef(), xpr::undef()), xpr::list(xpr::real(2), xpr::vector({5, 5, 5}), xpr::vector({5, 5, 5})))->eval()->str() , "xpr::Vector(50, 50, 50)" );
    BOOST_CHECK_EQUAL( f1->eval()->str() , "xpr::Vector(50, 50, 50)" );
    BOOST_CHECK_EQUAL( f1->eval()->code() , "xpr::vector({50, 50, 50})" );
    BOOST_CHECK_EQUAL( f1->eval()->json() , "[50,50,50]" );

    // reduce one element list

    ExpPtr f2 = reduce( xpr::add(), xpr::list( a_ ) );

    BOOST_CHECK_EQUAL( f2->str() , "xpr::Reduce(xpr::Add(?, ?), xpr::List(xpr::Real(2)))" );
    BOOST_CHECK_EQUAL( f2->code() , "xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list(xpr::real(2)))" );
    BOOST_CHECK_EQUAL( f2->json() , "{\"xpr::reduce\":[{\"xpr::add\":[\"xpr::undef\",\"xpr::undef\"]},{\"xpr::list\":[2]}]}" );
    BOOST_CHECK_EQUAL( xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list(xpr::real(2)))->eval()->str() , "xpr::Real(2)" );
    BOOST_CHECK_EQUAL( f2->eval()->str() , "xpr::Real(2)" );
    BOOST_CHECK_EQUAL( f2->eval()->code() , "xpr::real(2)" );
    BOOST_CHECK_EQUAL( f2->eval()->json() , "2" );

    // reduce empty element list

    ExpPtr f3 =  reduce( xpr::add(), xpr::list() );

    BOOST_CHECK_EQUAL( f3->str() , "xpr::Reduce(xpr::Add(?, ?), xpr::List())" );
    BOOST_CHECK_EQUAL( f3->code() , "xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list())" );
    BOOST_CHECK_EQUAL( f3->json() , "{\"xpr::reduce\":[{\"xpr::add\":[\"xpr::undef\",\"xpr::undef\"]},{\"xpr::list\":[]}]}" );
    BOOST_CHECK_EQUAL( xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::list())->eval()->str() , "xpr::List()" );
    BOOST_CHECK_EQUAL( f3->eval()->str() , "xpr::List()" );
    BOOST_CHECK_EQUAL( f3->eval()->code() , "xpr::list()" );
    BOOST_CHECK_EQUAL( f3->eval()->json() , "{\"xpr::list\":[]}" );

    // reduce vector

    ExpPtr f4 =  reduce( xpr::add(), x_ );

    BOOST_CHECK_EQUAL( f4->str() , "xpr::Reduce(xpr::Add(?, ?), xpr::Vector(5, 5, 5))" );
    BOOST_CHECK_EQUAL( f4->code() , "xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::vector({5, 5, 5}))" );
    BOOST_CHECK_EQUAL( f4->json() , "{\"xpr::reduce\":[{\"xpr::add\":[\"xpr::undef\",\"xpr::undef\"]},[5,5,5]]}" );
    BOOST_CHECK_EQUAL( xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::vector({5, 5, 5}))->eval()->str() , "xpr::Real(15)" );
    BOOST_CHECK_EQUAL( f4->eval()->str() , "xpr::Real(15)" );
    BOOST_CHECK_EQUAL( f4->eval()->code() , "xpr::real(15)" );
    BOOST_CHECK_EQUAL( f4->eval()->json() , "15" );

    // reduce empty vector

    ExpPtr f5 =  reduce( xpr::add(), xpr::vector({}) );
    BOOST_CHECK_EQUAL( f5->str() , "xpr::Reduce(xpr::Add(?, ?), xpr::Vector())" );
    BOOST_CHECK_EQUAL( f5->code() , "xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::vector({}))" );
    BOOST_CHECK_EQUAL( f5->json() , "{\"xpr::reduce\":[{\"xpr::add\":[\"xpr::undef\",\"xpr::undef\"]},[]]}" );
    BOOST_CHECK_EQUAL( xpr::reduce(xpr::add(xpr::undef(), xpr::undef()), xpr::vector({}))->eval()->str() , "xpr::Vector()" );
    BOOST_CHECK_EQUAL( f5->eval()->str() , "xpr::Vector()" );
    BOOST_CHECK_EQUAL( f5->eval()->code() , "xpr::vector({})" );
    BOOST_CHECK_EQUAL( f5->eval()->json() , "[]" );
}

BOOST_AUTO_TEST_CASE( test_predicates )
{
    // NotEqual

    ExpPtr f0 =  xpr::not_equal( a_ , b_ );

    BOOST_CHECK_EQUAL( f0->str() , "xpr::NotEqual(xpr::Real(2), xpr::Real(4))" );
    BOOST_CHECK_EQUAL( f0->code() , "xpr::not_equal(xpr::real(2), xpr::real(4))" );
    BOOST_CHECK_EQUAL( f0->json() , "{\"xpr::not_equal\":[2,4]}" );
    BOOST_CHECK_EQUAL( xpr::not_equal(xpr::real(2), xpr::real(4))->eval()->str() , "xpr::Boolean(true)" );
    BOOST_CHECK_EQUAL( f0->eval()->str() , "xpr::Boolean(true)" );
    BOOST_CHECK_EQUAL( f0->eval()->code() , "xpr::boolean(true)" );
    BOOST_CHECK_EQUAL( f0->eval()->json() , "true" );

    // Greater

    ExpPtr f1 =  xpr::greater( a_ , b_ );

    BOOST_CHECK_EQUAL( f1->str() , "xpr::Greater(xpr::Real(2), xpr::Real(4))" );
    BOOST_CHECK_EQUAL( f1->code() , "xpr::greater(xpr::real(2), xpr::real(4))" );
    BOOST_CHECK_EQUAL( f1->json() , "{\"xpr::greater\":[2,4]}" );
    BOOST_CHECK_EQUAL( xpr::greater(xpr::real(2), xpr::real(4))->eval()->str() , "xpr::Boolean(false)" );
    BOOST_CHECK_EQUAL( f1->eval()->str() , "xpr::Boolean(false)" );
    BOOST_CHECK_EQUAL( f1->eval()->code() , "xpr::boolean(false)" );
    BOOST_CHECK_EQUAL( f1->eval()->json() , "false" );

    // Less

    ExpPtr f2 =  xpr::less( a_ , b_ );

    BOOST_CHECK_EQUAL( f2->str() , "xpr::Less(xpr::Real(2), xpr::Real(4))" );
    BOOST_CHECK_EQUAL( f2->code() , "xpr::less(xpr::real(2), xpr::real(4))" );
    BOOST_CHECK_EQUAL( f2->json() , "{\"xpr::less\":[2,4]}" );
    BOOST_CHECK_EQUAL( xpr::less(xpr::real(2), xpr::real(4))->eval()->str() , "xpr::Boolean(true)" );
    BOOST_CHECK_EQUAL( f2->eval()->str() , "xpr::Boolean(true)" );
    BOOST_CHECK_EQUAL( f2->eval()->code() , "xpr::boolean(true)" );
    BOOST_CHECK_EQUAL( f2->eval()->json() , "true" );
}

BOOST_AUTO_TEST_CASE( test_filter )
{
    ExpPtr f3 = xpr::filter( xpr::greater( undef(), xpr::real(2) ),xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK_EQUAL( f3->str() , "xpr::Filter(xpr::Greater(?, xpr::Real(2)), xpr::List(xpr::Real(2), xpr::Real(4), xpr::Real(2), xpr::Real(4)))" );
    BOOST_CHECK_EQUAL( f3->code() , "xpr::filter(xpr::greater(xpr::undef(), xpr::real(2)), xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::real(4)))" );
    BOOST_CHECK_EQUAL( f3->json() , "{\"xpr::filter\":[{\"xpr::greater\":[\"xpr::undef\",2]},{\"xpr::list\":[2,4,2,4]}]}" );
    BOOST_CHECK_EQUAL( xpr::filter(xpr::greater(xpr::undef(), xpr::real(2)), xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::real(4)))->eval()->str() , "xpr::List(xpr::Real(4), xpr::Real(4))" );
    BOOST_CHECK_EQUAL( f3->eval()->str() , "xpr::List(xpr::Real(4), xpr::Real(4))" );
    BOOST_CHECK_EQUAL( f3->eval()->code() , "xpr::list(xpr::real(4), xpr::real(4))" );
    BOOST_CHECK_EQUAL( f3->eval()->json() , "{\"xpr::list\":[4,4]}" );
}

BOOST_AUTO_TEST_CASE( test_sqrt )
{
    ExpPtr e1 = xpr::sqrt( a_ );
    BOOST_CHECK_EQUAL( e1->str() , "xpr::Sqrt(xpr::Real(2))");
    BOOST_CHECK_EQUAL( e1->code() , "xpr::sqrt(xpr::real(2))" );
    BOOST_CHECK_EQUAL( e1->json() , "{\"xpr::sqrt\":2}" );
    BOOST_CHECK_EQUAL( e1->eval()->str() , "xpr::Real(1.41421)" );
    BOOST_CHECK_EQUAL( e1->eval()->code() , "xpr::real(1.41421)" );
    BOOST_CHECK_EQUAL( e1->eval()->json() , "1.41421" );

    ExpPtr e2 = xpr::sqrt( x_ );
    BOOST_CHECK_EQUAL( e2->str() , "xpr::Sqrt(xpr::Vector(5, 5, 5))" );
    BOOST_CHECK_EQUAL( e2->code() , "xpr::sqrt(xpr::vector({5, 5, 5}))" );
    BOOST_CHECK_EQUAL( e2->json() , "{\"xpr::sqrt\":[5,5,5]}" );
    BOOST_CHECK_EQUAL( e2->eval()->str() , "xpr::Vector(2.23607, 2.23607, 2.23607)" );
    BOOST_CHECK_EQUAL( e2->eval()->code() , "xpr::vector({2.23607, 2.23607, 2.23607})" );
    BOOST_CHECK_EQUAL( e2->eval()->json() , "[2.23607,2.23607,2.23607]" );

    ExpPtr e3 = xpr::map( xpr::sqrt(), xpr::list( a_, x_ ) );
    BOOST_CHECK_EQUAL( e3->str() , "xpr::Map(xpr::Sqrt(?), xpr::List(xpr::Real(2), xpr::Vector(5, 5, 5)))" );
    BOOST_CHECK_EQUAL( e3->code() , "xpr::map(xpr::sqrt(xpr::undef()), xpr::list(xpr::real(2), xpr::vector({5, 5, 5})))" );
    BOOST_CHECK_EQUAL( e3->json() , "{\"xpr::map\":[{\"xpr::sqrt\":\"xpr::undef\"},{\"xpr::list\":[2,[5,5,5]]}]}" );
    BOOST_CHECK_EQUAL( e3->eval()->str() , "xpr::List(xpr::Real(1.41421), xpr::Vector(2.23607, 2.23607, 2.23607))" );
    BOOST_CHECK_EQUAL( e3->eval()->code() , "xpr::list(xpr::real(1.41421), xpr::vector({2.23607, 2.23607, 2.23607}))" );
    BOOST_CHECK_EQUAL( e3->eval()->json() , "{\"xpr::list\":[1.41421,[2.23607,2.23607,2.23607]]}" );
}

BOOST_AUTO_TEST_CASE( test_bind )
{
    ExpPtr pred = xpr::bind<2>( xpr::greater(), xpr::real(2) );

    ExpPtr f0 = xpr::filter( pred, xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK_EQUAL( f0->str() , "xpr::Filter(xpr::Bind(xpr::Integer(2), xpr::Greater(?, ?), xpr::Real(2)), xpr::List(xpr::Real(2), xpr::Real(4), xpr::Real(2), xpr::Real(4)))" );
    BOOST_CHECK_EQUAL( f0->code() , "xpr::filter(xpr::bind<2>(xpr::greater(xpr::undef(), xpr::undef()), xpr::real(2)), xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::real(4)))" );
    BOOST_CHECK_EQUAL( f0->json() , "{\"xpr::filter\":[{\"xpr::bind\":[2,{\"xpr::greater\":[\"xpr::undef\",\"xpr::undef\"]},2]},{\"xpr::list\":[2,4,2,4]}]}" );
    BOOST_CHECK_EQUAL( xpr::filter(xpr::bind<2>(xpr::greater(xpr::undef(), xpr::undef()), xpr::real(2)), xpr::list(xpr::real(2), xpr::real(4), xpr::real(2), xpr::real(4)))->eval()->str() , "xpr::List(xpr::Real(4), xpr::Real(4))" );
    BOOST_CHECK_EQUAL( f0->eval()->str() , "xpr::List(xpr::Real(4), xpr::Real(4))" );
    BOOST_CHECK_EQUAL( f0->eval()->code() , "xpr::list(xpr::real(4), xpr::real(4))" );
    BOOST_CHECK_EQUAL( f0->eval()->json() , "{\"xpr::list\":[4,4]}" );
}

BOOST_AUTO_TEST_CASE( test_bind_fail )
{
    BOOST_TEST_MESSAGE("Binding to negative number should raise an assertion failure");
    BOOST_CHECK_THROW( xpr::bind<-2>( xpr::greater(), xpr::real(2) ), AssertionFailed);
}

BOOST_AUTO_TEST_CASE( test_zipwith )
{
    // simple reduce

    ExpPtr f0 =  zipWith( xpr::add(), xpr::list( a_ , b_, a_, b_ ), xpr::list( a_ , b_, a_, b_ ) );

    BOOST_CHECK_EQUAL( f0->eval()->str() , xpr::map( xpr::prod(real(2.)), xpr::list( a_ , b_, a_, b_ ) )->eval()->str() );
    BOOST_CHECK_EQUAL( f0->eval()->code() , "xpr::list(xpr::real(4), xpr::real(8), xpr::real(4), xpr::real(8))" );
    BOOST_CHECK_EQUAL( f0->eval()->json() , "{\"xpr::list\":[4,8,4,8]}" );

}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

} // namespace eckit_test
