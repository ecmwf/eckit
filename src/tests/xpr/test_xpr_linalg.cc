/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_xpr_linalg

#include "ecbuild/boost_test_framework.h"

#include "eckit/xpr/Accumulate.h"
#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/Dot.h"
#include "eckit/xpr/Integer.h"
#include "eckit/xpr/Matrix.h"
#include "eckit/xpr/Norm.h"
#include "eckit/xpr/Real.h"
#include "eckit/xpr/Vector.h"

using namespace eckit;
using namespace eckit::xpr;

//-----------------------------------------------------------------------------

namespace eckit_test {

//-----------------------------------------------------------------------------

/// Linear algebra tests for expression engine

struct ExpFixture {

    ExpFixture() : a( xpr::real( 2. ) ),
                   n( xpr::integer( 2 ) ),
                   x( xpr::vector( {1., 2., 4.} ) ),
                   m( xpr::matrix( 2, 2, {1., -2., -4., 2.} ) ) {}

    ExpPtr a;
    ExpPtr n;
    ExpPtr x;
    ExpPtr m;
};

template < class T >
void test(ExpPtr e, const std::initializer_list<real_t>& res)
{
    typename T::value_t v = T::extract(e->eval());
    typename T::value_t r(res);
    BOOST_CHECK_EQUAL_COLLECTIONS( v.begin(), v.end(), r.begin(), r.end() );
}

void test(ExpPtr e, real_t res)
{
    BOOST_CHECK_EQUAL( Real::extract(e->eval()), res );
}

BOOST_FIXTURE_TEST_SUITE( test_eckit_xpr_linalg, ExpFixture )

BOOST_AUTO_TEST_CASE( test_vector_add )
{
    test<Vector>(add(a, x), {3.,4.,6.});
    test<Vector>(add(n, x), {3.,4.,6.});
    test<Vector>(add(x, a), {3.,4.,6.});
    test<Vector>(add(x, n), {3.,4.,6.});
    test<Vector>(add(x, x), {2.,4.,8.});
    BOOST_TEST_MESSAGE("Adding vectors of different sizes should fail");
    BOOST_CHECK_THROW( add(x, vector({1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_vector_sub )
{
    test<Vector>(sub(a, x), {1.,0.,-2.});
    test<Vector>(sub(n, x), {1.,0.,-2.});
    test<Vector>(sub(x, a), {-1.,0.,2.});
    test<Vector>(sub(x, n), {-1.,0.,2.});
    test<Vector>(sub(x, x), {0.,0.,0.});
    BOOST_TEST_MESSAGE("Subtracting vectors of different sizes should fail");
    BOOST_CHECK_THROW( sub(x, vector({1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_vector_prod )
{
    test<Vector>(mul(a, x), {2.,4.,8.});
    test<Vector>(mul(n, x), {2.,4.,8.});
    test<Vector>(mul(x, a), {2.,4.,8.});
    test<Vector>(mul(x, n), {2.,4.,8.});
    test<Vector>(mul(x, x), {1.,4.,16.});
    BOOST_TEST_MESSAGE("Multiplying vectors of different sizes should fail");
    BOOST_CHECK_THROW( mul(x, vector({1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_vector_div )
{
    test<Vector>(div(a, x), {2.,1.,.5});
    test<Vector>(div(n, x), {2.,1.,.5});
    test<Vector>(div(x, a), {.5,1.,2.});
    test<Vector>(div(x, n), {.5,1.,2.});
    test<Vector>(div(x, x), {1.,1.,1.});
    BOOST_TEST_MESSAGE("Dividing vectors of different sizes should fail");
    BOOST_CHECK_THROW( div(x, vector({1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_vector_mod )
{
    test<Vector>(mod(a, x), {0.,0.,2.});
    test<Vector>(mod(n, x), {0.,0.,2.});
    test<Vector>(mod(x, a), {1.,0.,0.});
    test<Vector>(mod(x, n), {1.,0.,0.});
    test<Vector>(mod(x, x), {0.,0.,0.});
    BOOST_TEST_MESSAGE("Modulo operation on vectors of different sizes should fail");
    BOOST_CHECK_THROW( mod(x, vector({1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_vector_min )
{
    test<Vector>(min(a, x), {1.,2.,2.});
    test<Vector>(min(n, x), {1.,2.,2.});
    test<Vector>(min(x, a), {1.,2.,2.});
    test<Vector>(min(x, n), {1.,2.,2.});
    test<Vector>(min(x, x), {1.,2.,4.});
    BOOST_TEST_MESSAGE("Minimum of vectors of different sizes should fail");
    BOOST_CHECK_THROW( min(x, vector({1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_vector_max )
{
    test<Vector>(max(a, x), {2.,2.,4.});
    test<Vector>(max(n, x), {2.,2.,4.});
    test<Vector>(max(x, a), {2.,2.,4.});
    test<Vector>(max(x, n), {2.,2.,4.});
    test<Vector>(max(x, x), {1.,2.,4.});
    BOOST_TEST_MESSAGE("Maximum of vectors of different sizes should fail");
    BOOST_CHECK_THROW( add(x, vector({1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_matrix_add )
{
    test<Matrix>(add(a, m), {3.,0.,-2.,4.});
    test<Matrix>(add(n, m), {3.,0.,-2.,4.});
    test<Matrix>(add(m, a), {3.,0.,-2.,4.});
    test<Matrix>(add(m, n), {3.,0.,-2.,4.});
    test<Matrix>(add(m, m), {2.,-4.,-8.,4.});
    BOOST_TEST_MESSAGE("Adding matrices of different sizes should fail");
    BOOST_CHECK_THROW( add(m, matrix(1, 2, {1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_matrix_sub )
{
    test<Matrix>(sub(a, m), {1.,4.,6.,0.});
    test<Matrix>(sub(n, m), {1.,4.,6.,0.});
    test<Matrix>(sub(m, a), {-1.,-4.,-6.,0.});
    test<Matrix>(sub(m, n), {-1.,-4.,-6.,0.});
    test<Matrix>(sub(m, m), {0.,0.,0.,0.});
    BOOST_TEST_MESSAGE("Subtracting matrices of different sizes should fail");
    BOOST_CHECK_THROW( sub(m, matrix(1, 2, {1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_matrix_prod )
{
    test<Matrix>(mul(a, m), {2.,-4.,-8.,4.});
    test<Matrix>(mul(n, m), {2.,-4.,-8.,4.});
    test<Matrix>(mul(m, a), {2.,-4.,-8.,4.});
    test<Matrix>(mul(m, n), {2.,-4.,-8.,4.});
    test<Matrix>(mul(m, m), {1.,4.,16.,4.});
    BOOST_TEST_MESSAGE("Multiplying matrices of different sizes should fail");
    BOOST_CHECK_THROW( mul(m, matrix(1, 2, {1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_matrix_div )
{
    test<Matrix>(div(a, m), {2.,-1.,-.5,1.});
    test<Matrix>(div(n, m), {2.,-1.,-.5,1.});
    test<Matrix>(div(m, a), {.5,-1.,-2.,1.});
    test<Matrix>(div(m, n), {.5,-1.,-2.,1.});
    test<Matrix>(div(m, m), {1.,1.,1.,1.});
    BOOST_TEST_MESSAGE("Dividing matrices of different sizes should fail");
    BOOST_CHECK_THROW( div(m, matrix(1, 2, {1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_matrix_mod )
{
    test<Matrix>(mod(a, m), {0.,0.,2.,0.});
    test<Matrix>(mod(n, m), {0.,0.,2.,0.});
    test<Matrix>(mod(m, a), {1.,0.,0.,0.});
    test<Matrix>(mod(m, n), {1.,0.,0.,0.});
    test<Matrix>(mod(m, m), {0.,0.,0.,0.});
    BOOST_TEST_MESSAGE("Modulo operations on matrices of different sizes should fail");
    BOOST_CHECK_THROW( mod(m, matrix(1, 2, {1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_matrix_min )
{
    test<Matrix>(min(a, m), {1.,-2.,-4.,2.});
    test<Matrix>(min(n, m), {1.,-2.,-4.,2.});
    test<Matrix>(min(m, a), {1.,-2.,-4.,2.});
    test<Matrix>(min(m, n), {1.,-2.,-4.,2.});
    test<Matrix>(min(m, m), {1.,-2.,-4.,2.});
    BOOST_TEST_MESSAGE("Minimum of matrices of different sizes should fail");
    BOOST_CHECK_THROW( min(m, matrix(1, 2, {1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_matrix_max )
{
    test<Matrix>(max(a, m), {2.,2.,2.,2.});
    test<Matrix>(max(n, m), {2.,2.,2.,2.});
    test<Matrix>(max(m, a), {2.,2.,2.,2.});
    test<Matrix>(max(m, n), {2.,2.,2.,2.});
    test<Matrix>(max(m, m), {1.,-2.,-4.,2.});
    BOOST_TEST_MESSAGE("Maximum of matrices of different sizes should fail");
    BOOST_CHECK_THROW( max(m, matrix(1, 2, {1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_dot_vec_vec )
{
    BOOST_CHECK_EQUAL( Real::extract(dot(x, x)->eval()), 21. );
    BOOST_TEST_MESSAGE("Dot product of vectors of different sizes should fail");
    BOOST_CHECK_THROW( dot(x, vector({1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_dot_mat_vec )
{
    test<Vector>( dot(m, vector({-1., -2.})), {7., -2.} );
    BOOST_TEST_MESSAGE("Dot product of matrix and vector of nonmatching sizes should fail");
    BOOST_CHECK_THROW( dot(m, x)->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_dot_mat_mat )
{
    test<Matrix>( dot(m, m), {9., -6., -12., 12.} );
    BOOST_TEST_MESSAGE("Dot product of matrices of nonmatching sizes should fail");
    BOOST_CHECK_THROW( dot(m, matrix(1, 2, {1.,2.}))->eval(), AssertionFailed );
}

BOOST_AUTO_TEST_CASE( test_sum )
{
    test(sum(x), 7.0 );
}

BOOST_AUTO_TEST_CASE( test_product )
{
    test(product(x), 8.0 );
}

BOOST_AUTO_TEST_CASE( test_mean )
{
    test(mean(x), 7.0/3 );
}

BOOST_AUTO_TEST_CASE( test_variance )
{
    test(variance(vector({10.,2.})), 16.0 );
}

BOOST_AUTO_TEST_CASE( test_stddev )
{
    test(stddev(vector({10.,2.})), 4.0 );
}

BOOST_AUTO_TEST_CASE( test_norm )
{
    test(norm(vector({3.,4.})), 5.0 );
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

} // namespace eckit_test
