/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_xpr_foreach

#include "ecbuild/boost_test_framework.h"

#include "eckit/xpr/Call.h"
#include "eckit/xpr/ForEach.h"
#include "eckit/xpr/Iterable.h"
#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Map.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Undef.h"
#include "eckit/xpr/Xpr.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

struct Counter : Integer, Iterable
{
    Counter( value_t count = 0, value_t limit = 0 ) : Integer(count), limit_(limit) {}
    ExpPtr next() {
        if (limit_ && v_ >= limit_)
            return ExpPtr( new Undef() );
        return ExpPtr( new Integer(v_++) );
    }
private:
    value_t limit_;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_xpr_foreach )

/// test ForEach expressions

BOOST_AUTO_TEST_CASE( test_lambda_list )
{
    Xpr trice = call(lambda("a", Xpr(3.0) * Xpr("a")));
    Xpr l = xpr::forEach( xpr::list( 2., 4., 1. ), trice );

    BOOST_CHECK_EQUAL( l.str(), "ForEach(List(Real(2), Real(4), Real(1)), Call(Lambda(=(a), Prod(Real(3), _(a)))))" );
    BOOST_CHECK_EQUAL( l.code(), "xpr::forEach(xpr::list(xpr::real(2), xpr::real(4), xpr::real(1)), xpr::call(xpr::lambda(\"a\", xpr::prod(xpr::real(3), xpr::parameter(\"a\")))))" );
    BOOST_CHECK_EQUAL( l.json(), "{\"xpr::forEach\":[{\"xpr::list\":[2,4,1]},{\"xpr::call\":[{\"xpr::lambda\":[{\"xpr::paramdef\":[]},{\"xpr::prod\":[3,{\"xpr::parameter\":[]}]}]}]}]}" );

    BOOST_CHECK_EQUAL( l.eval().str(), "List(Real(6), Real(12), Real(3))" );
    BOOST_CHECK_EQUAL( l.eval().code(), "xpr::list(xpr::real(6), xpr::real(12), xpr::real(3))" );
    BOOST_CHECK_EQUAL( l.eval().json(), "{\"xpr::list\":[6,12,3]}" );
}

BOOST_AUTO_TEST_CASE( test_lambda_list_nested )
{
    Xpr trice = call(lambda("a", Xpr(3.0) * Xpr("a")));
    Xpr l = xpr::forEach( xpr::forEach( xpr::list( 2., 4., 1. ), trice ), trice );

    BOOST_CHECK_EQUAL( l.str(), "ForEach(ForEach(List(Real(2), Real(4), Real(1)), Call(Lambda(=(a), Prod(Real(3), _(a))))), Call(Lambda(=(a), Prod(Real(3), _(a)))))" );
    BOOST_CHECK_EQUAL( l.code(), "xpr::forEach(xpr::forEach(xpr::list(xpr::real(2), xpr::real(4), xpr::real(1)), xpr::call(xpr::lambda(\"a\", xpr::prod(xpr::real(3), xpr::parameter(\"a\"))))), xpr::call(xpr::lambda(\"a\", xpr::prod(xpr::real(3), xpr::parameter(\"a\")))))" );
    BOOST_CHECK_EQUAL( l.json(), "{\"xpr::forEach\":[{\"xpr::forEach\":[{\"xpr::list\":[2,4,1]},{\"xpr::call\":[{\"xpr::lambda\":[{\"xpr::paramdef\":[]},{\"xpr::prod\":[3,{\"xpr::parameter\":[]}]}]}]}]},{\"xpr::call\":[{\"xpr::lambda\":[{\"xpr::paramdef\":[]},{\"xpr::prod\":[3,{\"xpr::parameter\":[]}]}]}]}]}" );

    BOOST_CHECK_EQUAL( l.eval().str(), "List(Real(18), Real(36), Real(9))" );
    BOOST_CHECK_EQUAL( l.eval().code(), "xpr::list(xpr::real(18), xpr::real(36), xpr::real(9))" );
    BOOST_CHECK_EQUAL( l.eval().json(), "{\"xpr::list\":[18,36,9]}" );
}

BOOST_AUTO_TEST_CASE( test_lambda_list_map )
{
    Xpr trice = call(lambda("a", Xpr(3.0) * Xpr("a")));
    Xpr l = xpr::forEach( xpr::map( trice, xpr::list( 2., 4., 1. ) ), trice );

    BOOST_CHECK_EQUAL( l.str(), "ForEach(Map(Call(Lambda(=(a), Prod(Real(3), _(a)))), List(Real(2), Real(4), Real(1))), Call(Lambda(=(a), Prod(Real(3), _(a)))))" );
    BOOST_CHECK_EQUAL( l.code(), "xpr::forEach(xpr::map(xpr::call(xpr::lambda(\"a\", xpr::prod(xpr::real(3), xpr::parameter(\"a\")))), xpr::list(xpr::real(2), xpr::real(4), xpr::real(1))), xpr::call(xpr::lambda(\"a\", xpr::prod(xpr::real(3), xpr::parameter(\"a\")))))" );
    BOOST_CHECK_EQUAL( l.json(), "{\"xpr::forEach\":[{\"xpr::map\":[{\"xpr::call\":[{\"xpr::lambda\":[{\"xpr::paramdef\":[]},{\"xpr::prod\":[3,{\"xpr::parameter\":[]}]}]}]},{\"xpr::list\":[2,4,1]}]},{\"xpr::call\":[{\"xpr::lambda\":[{\"xpr::paramdef\":[]},{\"xpr::prod\":[3,{\"xpr::parameter\":[]}]}]}]}]}" );

    BOOST_CHECK_EQUAL( l.eval().str(), "List(Real(18), Real(36), Real(9))" );
    BOOST_CHECK_EQUAL( l.eval().code(), "xpr::list(xpr::real(18), xpr::real(36), xpr::real(9))" );
    BOOST_CHECK_EQUAL( l.eval().json(), "{\"xpr::list\":[18,36,9]}" );
}

BOOST_AUTO_TEST_CASE( test_lambda_iterable )
{
    Xpr trice = call(lambda("a", Xpr(3.0) * Xpr("a")));
    Xpr l = xpr::forEach( ExpPtr( new Counter(1, 4) ), trice );

    BOOST_CHECK_EQUAL( l.str(), "ForEach(Integer(1), Call(Lambda(=(a), Prod(Real(3), _(a)))))" );
    BOOST_CHECK_EQUAL( l.code(), "xpr::forEach(xpr::integer(1), xpr::call(xpr::lambda(\"a\", xpr::prod(xpr::real(3), xpr::parameter(\"a\")))))" );
    BOOST_CHECK_EQUAL( l.json(), "{\"xpr::forEach\":[1,{\"xpr::call\":[{\"xpr::lambda\":[{\"xpr::paramdef\":[]},{\"xpr::prod\":[3,{\"xpr::parameter\":[]}]}]}]}]}" );

    Xpr le = l.eval();

    BOOST_CHECK_EQUAL( le.str(), "List(Real(3), Real(6), Real(9))" );
    BOOST_CHECK_EQUAL( le.code(), "xpr::list(xpr::real(3), xpr::real(6), xpr::real(9))" );
    BOOST_CHECK_EQUAL( le.json(), "{\"xpr::list\":[3,6,9]}" );

    // Counter is exhausted after iterating it once
    BOOST_CHECK_EQUAL( l.eval().str(), "List()" );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
