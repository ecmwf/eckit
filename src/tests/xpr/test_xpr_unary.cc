/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_xpr_unary

#include "ecbuild/boost_test_framework.h"

#include "eckit/xpr/UnaryOperator.h"
#include "eckit/xpr/Integer.h"
#include "eckit/xpr/Matrix.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"

using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

/// Unary operator tests for expression engine

template < class T >
void test(ExpPtr e, const std::initializer_list<real_t>& res)
{
    typename T::value_t v = T::extract(e->eval());
    typename T::value_t r(res);
    BOOST_CHECK_EQUAL_COLLECTIONS( v.begin(), v.end(), r.begin(), r.end() );
}

void testr(ExpPtr e, real_t a)
{
    BOOST_CHECK_EQUAL( Real::extract(e->eval()), a );
}

void testi(ExpPtr e, integer_t a)
{
    BOOST_CHECK_EQUAL( Integer::extract(e->eval()), a );
}

BOOST_AUTO_TEST_SUITE( test_eckit_xpr_unary )

BOOST_AUTO_TEST_CASE( test_neg )
{
    testr( neg(real(5.)), -5.0 );
    testr( neg(real(-5.)), 5.0 );
    testi( neg(integer(5)), -5 );
    testi( neg(integer(-5)), 5 );
    test<Vector>(neg(vector({3.,-4.})), {-3.,4.});
    test<Matrix>(neg(matrix(2, 2, {1., -2., -4., 2.})), {-1., 2., 4., -2.});
}

BOOST_AUTO_TEST_CASE( test_sqrt )
{
    testr( sqrt(real(9.)), 3.0 );
    testi( sqrt(integer(10)), 3 );
    test<Vector>(sqrt(vector({4.,9.})), {2.,3.});
    test<Matrix>(sqrt(matrix(2, 2, {1., 4., 9., 16.})), {1., 2., 3., 4.});
}

BOOST_AUTO_TEST_CASE( test_abs )
{
    testr( abs(real(5.)), 5.0 );
    testr( abs(real(-5.)), 5.0 );
    testi( abs(integer(5)), 5 );
    testi( abs(integer(-5)), 5 );
    test<Vector>(abs(vector({3.,-4.})), {3.,4.});
    test<Matrix>(abs(matrix(2, 2, {1., -2., -4., 2.})), {1., 2., 4., 2.});
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

} // namespace eckit_test
