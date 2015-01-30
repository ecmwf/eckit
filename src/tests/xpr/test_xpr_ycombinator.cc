/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iomanip>

#define BOOST_TEST_MODULE test_eckit_xpr_ycombinator

#include "ecbuild/boost_test_framework.h"

#include "eckit/xpr/Call.h"
#include "eckit/xpr/IfElse.h"
#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/ParamDef.h"
#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/Take.h"
#include "eckit/xpr/Xpr.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

/// Test if expression engine can define a y-combinator

struct XprFixture {

    XprFixture()
        : Y( lambda("f", call(Xpr("f"), Xpr("f"))) ) {}

    Xpr Y;
};

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE( test_eckit_xpr_expr, XprFixture )

BOOST_AUTO_TEST_CASE( test_ycombinator_factorial )
{
    // define the factorial (non-recursive) lambda function

    Xpr fact = lambda("f",
                      call(lambda("n",
                                  ifelse(
                                      Xpr("n") == Xpr(0.0),
                                      Xpr(1.0),
                                      Xpr("n") * call( Xpr("f"), Xpr("f"), Xpr("n") - Xpr(1.0))
                                      )
                                  )
                           )
                      );

    // use y-combinator to recurse the function

    Xpr factorial = call(Y, fact);

//    setformat(cout, xpr::CodeFormat);
    BOOST_TEST_MESSAGE( "factorial: " << factorial );
//    setformat(cout, 0);

    // compute some values

    Xpr fac3 = factorial( 3. );
    Xpr fac5 = factorial( 5. );
    Xpr fac8 = factorial( 8. );

    // check results values

    BOOST_CHECK( Scalar::extract(fac3) == 6 );
    BOOST_CHECK( Scalar::extract(fac5) == 120 );
    BOOST_CHECK( Scalar::extract(fac8) == 40320 );

    BOOST_TEST_MESSAGE( "factorial(3) = " << fac3 );
    BOOST_TEST_MESSAGE( "factorial(5) = " << fac5 );
    BOOST_TEST_MESSAGE( "factorial(8) = " << fac8 );
}

BOOST_AUTO_TEST_CASE( test_ycombinator_fibonacci )
{
    // define the fibonacci lookup-table to speed-up the lower order evaluations

    Xpr fibtable = xpr::list( Xpr(0.), Xpr(1.), Xpr(1.), Xpr(2.), Xpr(3.), Xpr(5.),
                              Xpr(8.), Xpr(13.), Xpr(21.), Xpr(34.), Xpr(55.) );

    // define the factorial (non-recursive) lambda function

    Xpr fib = lambda("f",
                     call(lambda("n",
                                 ifelse(
                                     Xpr("n") > Xpr(10.),
                                     Xpr(call(Xpr("f"), Xpr("f"), Xpr("n") - Xpr(1.0))) +
                                     Xpr(call(Xpr("f"), Xpr("f"), Xpr("n") - Xpr(2.0))),
                                     take( Xpr("n"), fibtable )
                                     )
                                  )
                           )
                      );

    // use y-combinator to recurse the function

    Xpr fibonnaci = call(Y, fib);

    // compute some fibonacci numbers

    Xpr f5  = fibonnaci( 5. );
    Xpr f10 = fibonnaci( 10.);
    Xpr f20 = fibonnaci( 20.);
//    Xpr f30 = fibonnaci( 30.); // -- rather slow test

    // check results values

    BOOST_CHECK( Scalar::extract(f5)  == 5 );
    BOOST_CHECK( Scalar::extract(f10) == 55 );
    BOOST_CHECK( Scalar::extract(f20) == 6765 );
//    BOOST_CHECK( Scalar::extract(f30) == 832040 ); // -- rather slow test

    BOOST_TEST_MESSAGE( "fibonnaci(3) = " << f5 );
    BOOST_TEST_MESSAGE( "fibonnaci(10) = " << f10 );
    BOOST_TEST_MESSAGE( "fibonnaci(20) = " << f20 );
//    BOOST_TEST_MESSAGE( "fibonnaci(30) = " << f30 );
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

} // namespace eckit_test
