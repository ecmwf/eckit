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

#include "eckit/xpr/IfElse.h"
#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/ParamDef.h"
#include "eckit/xpr/Call.h"
#include "eckit/xpr/Bind.h"

#include "eckit/xpr/Xpr.h"
#include "eckit/xpr/Scalar.h"

using namespace std;
using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_xpr_currying )

/// Test if expression engine can curry parameters

BOOST_AUTO_TEST_CASE( test_2args )
{
    Xpr X = lambda("i", "j", Xpr("i") + Xpr("j"));
    Xpr Y = call(X);

    Xpr r = Y(1.0, 2.4);

    BOOST_TEST_MESSAGE( "Y: " << Y );
    BOOST_TEST_MESSAGE( "r: " << r );

    BOOST_CHECK( Scalar::extract(r) == 3.4 );
}

BOOST_AUTO_TEST_CASE( test_twice_1arg )
{
    Xpr X = lambda("i", call( lambda("j", Xpr("i") * Xpr("j"))));
    Xpr Y = call(X, Xpr( 2. ));

    Xpr r = Y( 3. );

    BOOST_TEST_MESSAGE( "Y: " << Y );
    BOOST_TEST_MESSAGE( "r: " << r );

    BOOST_CHECK( Scalar::extract(r) == 6. );
}

BOOST_AUTO_TEST_CASE( test_with_bind )
{
    Xpr X = lambda("i", "j", Xpr("i") * Xpr("j"));
    Xpr Q = xpr::bind<2>(call(X), Xpr( 4. ));

    Xpr r = Q( 2. );

    BOOST_TEST_MESSAGE( "r: " << r );

    BOOST_CHECK( Scalar::extract(r) == 8. );
}

BOOST_AUTO_TEST_CASE( test_twice_lambda )
{
    Xpr C = call(
                  lambda("i",
                         call(
                             lambda("j",
                                      (Xpr("i") - Xpr("j")
                                       )
                                    )
                             )
                         ),
                 Xpr(2.0)); // bound to 'i'

    Xpr r = C( 4.0 ); // value for 'j'

    BOOST_TEST_MESSAGE( "C: " << C );
    BOOST_TEST_MESSAGE( "r: " << r );

    BOOST_CHECK( Scalar::extract(r) == -2. );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
