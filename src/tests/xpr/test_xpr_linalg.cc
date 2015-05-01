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

#include "eckit/xpr/BinaryOperator.h"
#include "eckit/xpr/Integer.h"
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
                   x( xpr::vector( {1., 2., 4.} ) ) {}

    ExpPtr a;
    ExpPtr n;
    ExpPtr x;
};

void test_vector(ExpPtr e, const std::initializer_list<real_t>& res)
{
    Vector::value_t v = Vector::extract(e->eval());
    Vector::value_t r(res);
    BOOST_CHECK_EQUAL_COLLECTIONS( v.begin(), v.end(), r.begin(), r.end() );
}

BOOST_FIXTURE_TEST_SUITE( test_eckit_xpr_linalg, ExpFixture )

BOOST_AUTO_TEST_CASE( test_vector_add )
{
    test_vector(add(a, x), {3.,4.,6.});
    test_vector(add(n, x), {3.,4.,6.});
    test_vector(add(x, a), {3.,4.,6.});
    test_vector(add(x, n), {3.,4.,6.});
    test_vector(add(x, x), {2.,4.,8.});
}

BOOST_AUTO_TEST_CASE( test_vector_sub )
{
    test_vector(sub(a, x), {1.,0.,-2.});
    test_vector(sub(n, x), {1.,0.,-2.});
    test_vector(sub(x, a), {-1.,0.,2.});
    test_vector(sub(x, n), {-1.,0.,2.});
    test_vector(sub(x, x), {0.,0.,0.});
}

BOOST_AUTO_TEST_CASE( test_vector_prod )
{
    test_vector(prod(a, x), {2.,4.,8.});
    test_vector(prod(n, x), {2.,4.,8.});
    test_vector(prod(x, a), {2.,4.,8.});
    test_vector(prod(x, n), {2.,4.,8.});
    test_vector(prod(x, x), {1.,4.,16.});
}

BOOST_AUTO_TEST_CASE( test_vector_div )
{
    test_vector(div(a, x), {2.,1.,.5});
    test_vector(div(n, x), {2.,1.,.5});
    test_vector(div(x, a), {.5,1.,2.});
    test_vector(div(x, n), {.5,1.,2.});
    test_vector(div(x, x), {1.,1.,1.});
}

BOOST_AUTO_TEST_CASE( test_vector_mod )
{
    test_vector(mod(a, x), {0.,0.,2.});
    test_vector(mod(n, x), {0.,0.,2.});
    test_vector(mod(x, a), {1.,0.,0.});
    test_vector(mod(x, n), {1.,0.,0.});
    test_vector(mod(x, x), {0.,0.,0.});
}

BOOST_AUTO_TEST_CASE( test_vector_min )
{
    test_vector(min(a, x), {1.,2.,2.});
    test_vector(min(n, x), {1.,2.,2.});
    test_vector(min(x, a), {1.,2.,2.});
    test_vector(min(x, n), {1.,2.,2.});
    test_vector(min(x, x), {1.,2.,4.});
}

BOOST_AUTO_TEST_CASE( test_vector_max )
{
    test_vector(max(a, x), {2.,2.,4.});
    test_vector(max(n, x), {2.,2.,4.});
    test_vector(max(x, a), {2.,2.,4.});
    test_vector(max(x, n), {2.,2.,4.});
    test_vector(max(x, x), {1.,2.,4.});
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

} // namespace eckit_test
