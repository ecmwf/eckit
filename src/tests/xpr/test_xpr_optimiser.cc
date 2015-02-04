/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_xpr_optimiser

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

/// Test if expression engine can optimise expression trees without evaluating them

struct XprFixture {

    XprFixture() : a( 2 ), b( 4. ), x({5, 5, 5}), y({7, 7, 7}) {}

    Xpr a;
    Xpr b;
    Xpr x;
    Xpr y;
};

//-----------------------------------------------------------------------------

BOOST_FIXTURE_TEST_SUITE( test_eckit_xpr_optimiser, XprFixture )

BOOST_AUTO_TEST_CASE( test_add_vv )
{
    // should not optimise

    Xpr X = x + y;

    BOOST_CHECK( X.optimise().expr()->str() == "Add(Vector(5, 5, 5), Vector(7, 7, 7))" );
    BOOST_CHECK( X.optimise().expr()->code() == "xpr::add(xpr::vector({5, 5, 5}), xpr::vector({7, 7, 7}))" );
    BOOST_CHECK( X.optimise().expr()->json() == "{\"xpr::add\":[[5,5,5],[7,7,7]]}" );
}

BOOST_AUTO_TEST_CASE( test_list_add_ss_vv )
{
    Xpr X = xpr::list( a + b, x + y);

    BOOST_CHECK( X.optimise().expr()->str() == "List(Scalar(6), Add(Vector(5, 5, 5), Vector(7, 7, 7)))" );
    BOOST_CHECK( X.optimise().expr()->code() == "xpr::list(xpr::scalar(6), xpr::add(xpr::vector({5, 5, 5}), xpr::vector({7, 7, 7})))" );
    BOOST_CHECK( X.optimise().expr()->json() == "{\"xpr::list\":[6,{\"xpr::add\":[[5,5,5],[7,7,7]]}]}" );
}

BOOST_AUTO_TEST_CASE( test_linear_sv_sv )
{
    Xpr e = a + b;
    Xpr z = (a - b) * e;
    Xpr X = e * x + z * y;

    BOOST_CHECK( X.optimise().expr()->str() == "Linear(Scalar(6), Vector(5, 5, 5), Scalar(-12), Vector(7, 7, 7))" );
    BOOST_CHECK( X.optimise().expr()->code() == "xpr::linear(xpr::scalar(6), xpr::vector({5, 5, 5}), xpr::scalar(-12), xpr::vector({7, 7, 7})))" );
    BOOST_CHECK( X.optimise().expr()->json() == "{\"xpr::linear\":[6,[5,5,5],-12,[7,7,7]]}" );
}

BOOST_AUTO_TEST_CASE( test_linear_with_count )
{
    Xpr e = xpr::count(
                xpr::merge(
                    xpr::list(a,a,a,a,a,a,a),
                           xpr::list(b,b,b))
                             );
    Xpr X = e * x + b * y;

    BOOST_CHECK( X.optimise().expr()->str() == "Linear(Scalar(10), Vector(5, 5, 5), Scalar(4), Vector(7, 7, 7))" );
    BOOST_CHECK( X.optimise().expr()->code() == "xpr::linear(xpr::scalar(10), xpr::vector({5, 5, 5}), xpr::scalar(4), xpr::vector({7, 7, 7})))" );
    BOOST_CHECK( X.optimise().expr()->json() == "{\"xpr::linear\":[10,[5,5,5],4,[7,7,7]]}" );
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

} // namespace eckit_test
