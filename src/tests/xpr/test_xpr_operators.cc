/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_xpr_operators

#include "ecbuild/boost_test_framework.h"

#include "eckit/xpr/Boolean.h"
#include "eckit/xpr/Count.h"
#include "eckit/xpr/List.h"
#include "eckit/xpr/Merge.h"
#include "eckit/xpr/Scalar.h"
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

BOOST_FIXTURE_TEST_SUITE( test_eckit_xpr_operators, XprFixture )

#define test_operator(OP, OPSM, SYM, RESULT, TYPE, TYPESM) \
BOOST_AUTO_TEST_CASE( test_##OPSM##_ss ) \
{ \
    Xpr A = a SYM b; \
    Xpr B = 2 SYM b; \
    Xpr C = a SYM 4.; \
    Xpr D = 2 SYM 4.; \
\
    BOOST_CHECK_EQUAL( extract<TYPE>(A.eval()) , RESULT ); \
    BOOST_CHECK_EQUAL( extract<TYPE>(B.eval()) , RESULT ); \
    BOOST_CHECK_EQUAL( extract<TYPE>(C.eval()) , RESULT ); \
    BOOST_CHECK_EQUAL( extract<TYPE>(D.eval()) , RESULT ); \
\
    BOOST_CHECK_EQUAL( A.expr()->str() , #OP "(Scalar(2), Scalar(4))" ); \
    BOOST_CHECK_EQUAL( B.expr()->str() , #OP "(Scalar(2), Scalar(4))" ); \
    BOOST_CHECK_EQUAL( C.expr()->str() , #OP "(Scalar(2), Scalar(4))" ); \
    BOOST_CHECK_EQUAL( D.expr()->str() , #TYPE "(" #RESULT ")" ); \
\
    BOOST_CHECK_EQUAL( A.expr()->code() , "xpr::" #OPSM "(xpr::scalar(2), xpr::scalar(4))" ); \
    BOOST_CHECK_EQUAL( B.expr()->code() , "xpr::" #OPSM "(xpr::scalar(2), xpr::scalar(4))" ); \
    BOOST_CHECK_EQUAL( C.expr()->code() , "xpr::" #OPSM "(xpr::scalar(2), xpr::scalar(4))" ); \
    BOOST_CHECK_EQUAL( D.expr()->code() , "xpr::" #TYPESM "(" #RESULT ")" ); \
\
    BOOST_CHECK_EQUAL( A.expr()->json() , "{\"xpr::" #OPSM "\":[2,4]}" ); \
    BOOST_CHECK_EQUAL( B.expr()->json() , "{\"xpr::" #OPSM "\":[2,4]}" ); \
    BOOST_CHECK_EQUAL( C.expr()->json() , "{\"xpr::" #OPSM "\":[2,4]}" ); \
    BOOST_CHECK_EQUAL( D.expr()->json() , #RESULT ); \
}

test_operator( Add, add, +, 6, Scalar, scalar )
test_operator( Sub, sub, -, -2, Scalar, scalar )
test_operator( Div, div, /, 0.5, Scalar, scalar )
test_operator( Prod, prod, *, 8, Scalar, scalar )
test_operator( Equal, equal, ==, false, Boolean, boolean )
test_operator( NotEqual, not_equal, !=, true, Boolean, boolean )
test_operator( Greater, greater, >, false, Boolean, boolean )
test_operator( Less, less, <, true, Boolean, boolean )
test_operator( GreaterEqual, greater_equal, >=, false, Boolean, boolean )
test_operator( LessEqual, less_equal, <=, true, Boolean, boolean )

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

} // namespace eckit_test
