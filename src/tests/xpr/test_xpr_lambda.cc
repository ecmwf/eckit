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

#include "eckit/xpr/IfElse.h"
#include "eckit/xpr/Lambda.h"
#include "eckit/xpr/ParamDef.h"
#include "eckit/xpr/Call.h"
#include "eckit/xpr/Bind.h"

#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/Count.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Merge.h"
#include "eckit/xpr/Scalar.h"
#include "eckit/xpr/Vector.h"
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
    setformat(std::cout,xpr::CodeFormat);

    Xpr a = xpr::scalar( 2. );
    Xpr b = xpr::scalar( 4. );
    Xpr c = xpr::scalar( 1. );

    Xpr l = xpr::list(a, b, c);

    Xpr twice  = call(lambda("a", Xpr(2.0) * Xpr("a")));

    BOOST_TEST_MESSAGE( "twice: " << twice(c) );

    BOOST_CHECK( Scalar::extract(twice(c)) == 2. );

    Xpr neg    = call(lambda("a", Xpr(0.0) - Xpr("a")));

    BOOST_TEST_MESSAGE( "neg: " << neg(a) );

    BOOST_CHECK( Scalar::extract(neg(a)) == -2. );

    Xpr X = l + l;

    BOOST_TEST_MESSAGE( "X: " << X );
    BOOST_TEST_MESSAGE( "X string representation: " << X.expr()->str() );


    X = X(); // evaluation to self

    BOOST_TEST_MESSAGE( "== After evaluation ==" );
    BOOST_TEST_MESSAGE( "X: " << X );
    BOOST_TEST_MESSAGE( "X string representation: " << X.expr()->str() );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
