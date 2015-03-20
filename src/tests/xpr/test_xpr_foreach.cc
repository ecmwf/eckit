/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_xpr_lambda

#include "ecbuild/boost_test_framework.h"

#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/Call.h"

#include "eckit/xpr/ForEach.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Xpr.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_xpr_lambda )

/// test lambda expressions

BOOST_AUTO_TEST_CASE( test_lambda )
{
    Xpr a = xpr::real( 2. );
    Xpr b = xpr::real( 4. );
    Xpr c = xpr::real( 1. );

    Xpr l = xpr::list( a, b, c );

    Xpr trice = call(lambda("a", Xpr(3.0) * Xpr("a")));

    ExpPtr loop = xpr::forEach( l, trice );

    BOOST_CHECK_EQUAL( loop->str(), "ForEach(List(Real(2), Real(4), Real(1)), Call(Lambda(=(a), Prod(Real(3), _(a)))))" );
    BOOST_CHECK_EQUAL( loop->code(), "xpr::forEach(xpr::list(xpr::real(2), xpr::real(4), xpr::real(1)), xpr::call(xpr::lambda(\"a\", xpr::prod(xpr::real(3), xpr::parameter(\"a\")))))" );
    BOOST_CHECK_EQUAL( loop->json(), "{\"xpr::forEach\":[{\"xpr::list\":[2,4,1]},{\"xpr::call\":[{\"xpr::lambda\":[{\"xpr::paramdef\":[]},{\"xpr::prod\":[3,{\"xpr::parameter\":[]}]}]}]}]}" );

    BOOST_CHECK_EQUAL( loop->eval()->str(), "List(Real(6), Real(12), Real(3))" );
    BOOST_CHECK_EQUAL( loop->eval()->code(), "xpr::list(xpr::real(6), xpr::real(12), xpr::real(3))" );
    BOOST_CHECK_EQUAL( loop->eval()->json(), "{\"xpr::list\":[6,12,3]}" );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
