/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#define BOOST_TEST_MODULE test_eckit_types

#include "ecbuild/boost_test_framework.h"

#include "eckit/types/FloatCompare.h"

#include "eckit/testing/Setup.h"

using namespace eckit;
using namespace eckit::testing;

namespace {

bool is_equal(double a, double b, double epsilon) {
    return FloatCompare<double>::isApproximatelyEqual(a, b, epsilon);
}

bool is_equal(double a, double b) {
    return FloatCompare<double>::isApproximatelyEqual(a, b, 0.00001);
}

const double dEps = std::numeric_limits<double>::epsilon();
const double dInf = std::numeric_limits<double>::infinity();
const double dMin = std::numeric_limits<double>::min();
const double dMax = std::numeric_limits<double>::max();
const double qNaN = std::numeric_limits<double>::quiet_NaN();
const double sNaN = std::numeric_limits<double>::signaling_NaN();

};

//-----------------------------------------------------------------------------

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE( test_eckit_floatcompare )

BOOST_AUTO_TEST_CASE( test_large_numbers )
{
   BOOST_TEST_MESSAGE( "test_large_numbers" );

   BOOST_CHECK(  is_equal(1000000,       1000000      ));
   BOOST_CHECK(  is_equal(1000000,       1000000.00001));
   BOOST_CHECK(  is_equal(1000000.00001, 1000000      ));

   BOOST_CHECK(! is_equal(1000000.0,     1000001.0    ));
   BOOST_CHECK(! is_equal(1000001.0,     1000000.0    ));

   // -----------------------------------------------
   BOOST_CHECK(is_equal(dMax, dMax));
   BOOST_CHECK(is_equal(dMax, dMax, dEps));

   BOOST_CHECK(is_equal(dMin, dMin));
   BOOST_CHECK(is_equal(dMin, dMin, dEps));
}

BOOST_AUTO_TEST_CASE( test_negative_large_numbers )
{
   BOOST_TEST_MESSAGE( "test_negative_large_numbers " << dMin );

   BOOST_CHECK(  is_equal(-1000000,       -1000000      ));
   BOOST_CHECK(  is_equal(-1000000,       -1000000.00001));
   BOOST_CHECK(  is_equal(-1000000.00001, -1000000      ));

   BOOST_CHECK(! is_equal(-1000000.0,     -1000001.0    ));
   BOOST_CHECK(! is_equal(-1000001.0,     -1000000.0    ));

   // -----------------------------------------------
   BOOST_CHECK(is_equal(-dMax, -dMax      ));
   BOOST_CHECK(is_equal(-dMax, -dMax, dEps));

   BOOST_CHECK(is_equal(-dMin, -dMin      ));
   BOOST_CHECK(is_equal(-dMin, -dMin, dEps));
}

BOOST_AUTO_TEST_CASE( test_numbers_around_one )
{
   BOOST_TEST_MESSAGE( "test_numbers_around_one" );

   BOOST_CHECK(  is_equal(1.0000001, 1.0000002));
   BOOST_CHECK(  is_equal(1.0000002, 1.0000001));

   BOOST_CHECK(  is_equal(1.12345,   1.12346  ));
   BOOST_CHECK(  is_equal(1.12345,   1.12344, 0.001));

   BOOST_CHECK(! is_equal(1.0001,    1.0002   ));
   BOOST_CHECK(! is_equal(1.0002,    1.0001   ));
}

BOOST_AUTO_TEST_CASE( test_numbers_around_negative_one )
{
   BOOST_TEST_MESSAGE( "test_numbers_around_negative_one" );

   BOOST_CHECK(  is_equal(-1.0000001, -1.0000002));
   BOOST_CHECK(  is_equal(-1.0000002, -1.0000001));

   BOOST_CHECK(! is_equal(-1.0001,    -1.0002   ));
   BOOST_CHECK(! is_equal(-1.0002,    -1.0001   ));
}

BOOST_AUTO_TEST_CASE( test_numbers_between_one_and_zero )
{
   BOOST_TEST_MESSAGE( "test_numbers_between_one_and_zero" );

   BOOST_CHECK(  is_equal(0.000000001000001, 0.000000001000002));
   BOOST_CHECK(  is_equal(0.000000001000002, 0.000000001000001));

   BOOST_CHECK(! is_equal(0.00102,           0.00101          ));
   BOOST_CHECK(! is_equal(0.00101,           0.00102          ));

}

BOOST_AUTO_TEST_CASE( test_numbers_between_minusone_and_zero )
{
   BOOST_TEST_MESSAGE( "test_numbers_between_minusone_and_zero" );

   BOOST_CHECK(  is_equal(-0.000000001000001, -0.000000001000002));
   BOOST_CHECK(  is_equal(-0.000000001000002, -0.000000001000001));

   BOOST_CHECK(! is_equal(-0.00102,           -0.00101          ));
   BOOST_CHECK(! is_equal(-0.00101,           -0.00102          ));
}

BOOST_AUTO_TEST_CASE( test_comparisons_involving_zero )
{
   BOOST_TEST_MESSAGE( "test_comparisons_involving_zero" );

   BOOST_CHECK(  is_equal(0.0,     0.0   ));
   BOOST_CHECK(  is_equal(0.0,    -0.0   ));
   BOOST_CHECK(  is_equal(-0.0,   -0.0   ));

   BOOST_CHECK(! is_equal(0.0001,  0.0   ));
   BOOST_CHECK(! is_equal(0.0,     0.0001));
   BOOST_CHECK(! is_equal(-0.0001, 0.0   ));
   BOOST_CHECK(! is_equal(0.0,    -0.0001));

   BOOST_CHECK(  is_equal(0.0,     1e-40, 0.01 ));
   BOOST_CHECK(  is_equal(1e-40,   0.0,   0.01 ));
   BOOST_CHECK(! is_equal(1e-40,   0.0,   1e-41));
   BOOST_CHECK(! is_equal(0.0,     1e-40, 1e-41));

   BOOST_CHECK(  is_equal(0.0,    -1e-40, 0.1  ));
   BOOST_CHECK(  is_equal(-1e-40,  0.0,   0.1  ));
   BOOST_CHECK(! is_equal(-1e-40,  0.0,   1e-41));
   BOOST_CHECK(! is_equal(0.0,    -1e-40, 1e-41));
}

BOOST_AUTO_TEST_CASE( test_comparisons_involving_infinity )
{
   BOOST_TEST_MESSAGE( "test_comparisons_involving_infinity" );

   if (std::numeric_limits<double>::has_infinity) {

      BOOST_CHECK(is_equal( dInf,  dInf));
      BOOST_CHECK(is_equal(-dInf, -dInf));
   }
   else {
      BOOST_TEST_MESSAGE( "test_comparisons_involving_infinity NOT VALID on this platform" );
   }
}

BOOST_AUTO_TEST_CASE( test_comparisons_involving_nan )
{
    BOOST_TEST_MESSAGE( "test_comparisons_involving_nan" );

    // The value NaN (Not a Number) is used to represent a value that does not represent a real number.
    // NaN's are represented by a bit pattern with an exponent of all 1s and a non-zero fraction. T
    // there are two categories of NaN: QNaN (Quiet NaN) and SNaN (Signalling NaN).
    //
    // A QNaN is a NaN with the most significant fraction bit set.
    // QNaN's propagate freely through most arithmetic operations.
    // These values pop out of an operation when the result is not mathematically defined.

    // An SNaN is a NaN with the most significant fraction bit clear.
    // It is used to signal an exception when used in operations.
    // SNaN's can be handy to assign to uninitialized variables to trap premature usage.

    // Semantically, QNaN's denote indeterminate operations, while SNaN's denote invalid operations.

    BOOST_CHECK(! is_equal( qNaN,  qNaN));
    BOOST_CHECK(! is_equal( qNaN,  0.0 ));
    BOOST_CHECK(! is_equal(-0.0 ,  qNaN));
    BOOST_CHECK(! is_equal( qNaN, -0.0 ));
    BOOST_CHECK(! is_equal( 0.0 ,  qNaN));
    BOOST_CHECK(! is_equal( qNaN,  dInf));
    BOOST_CHECK(! is_equal( dInf,  qNaN));
    BOOST_CHECK(! is_equal( qNaN,  dMax));
    BOOST_CHECK(! is_equal( dMax,  qNaN));
    BOOST_CHECK(! is_equal( qNaN, -dMax));
    BOOST_CHECK(! is_equal(-dMax,  qNaN));
    BOOST_CHECK(! is_equal( qNaN,  dMin));
    BOOST_CHECK(! is_equal( dMin,  qNaN));
    BOOST_CHECK(! is_equal( qNaN, -dMin));
    BOOST_CHECK(! is_equal(-dMin,  qNaN));

    BOOST_CHECK(! is_equal( sNaN,  sNaN));
    BOOST_CHECK(! is_equal( sNaN,  0.0 ));
    BOOST_CHECK(! is_equal(-0.0 ,  sNaN));
    BOOST_CHECK(! is_equal( sNaN, -0.0 ));
    BOOST_CHECK(! is_equal( 0.0 ,  sNaN));
    BOOST_CHECK(! is_equal( sNaN,  dInf));
    BOOST_CHECK(! is_equal( dInf,  sNaN));
    BOOST_CHECK(! is_equal( sNaN,  dMax));
    BOOST_CHECK(! is_equal( dMax,  sNaN));
    BOOST_CHECK(! is_equal( sNaN, -dMax));
    BOOST_CHECK(! is_equal(-dMax,  sNaN));
    BOOST_CHECK(! is_equal( sNaN,  dMin));
    BOOST_CHECK(! is_equal( dMin,  sNaN));
    BOOST_CHECK(! is_equal( sNaN, -dMin));
    BOOST_CHECK(! is_equal(-dMin,  sNaN));
}

BOOST_AUTO_TEST_CASE( test_comparisons_opposite_side_of_zero )
{
   BOOST_TEST_MESSAGE( "test_comparisons_opposite_side_of_zero" );

   BOOST_CHECK(! is_equal(1.000000001, -1.0        ));
   BOOST_CHECK(! is_equal(-1.0,         1.000000001));
   BOOST_CHECK(! is_equal(-1.000000001, 1.0        ));
   BOOST_CHECK(! is_equal(1.0,         -1.000000001));

   BOOST_CHECK(  is_equal(   10.0 * dMin,    10.0 * -dMin));
   BOOST_CHECK(  is_equal(10000   * dMin, 10000   * -dMin));
}

BOOST_AUTO_TEST_CASE( test_comparisons_very_close_to_zero )
{
   BOOST_TEST_MESSAGE( "test_comparisons_very_close_to_zero" );

   BOOST_CHECK(  is_equal( dMin, -dMin));
   BOOST_CHECK(  is_equal(-dMin,  dMin));
   BOOST_CHECK(  is_equal( dMin,  0   ));
   BOOST_CHECK(  is_equal( 0,     dMin));
   BOOST_CHECK(  is_equal(-dMin,  0   ));
   BOOST_CHECK(  is_equal( 0,    -dMin));


   BOOST_CHECK(  is_equal( 0.000000001, -dMin       ));
   BOOST_CHECK(  is_equal( 0.000000001,  dMin       ));
   BOOST_CHECK(  is_equal( dMin,         0.000000001));
   BOOST_CHECK(  is_equal(-dMin,         0.000000001));


   BOOST_CHECK(! is_equal( 0.000000001, -dMin,        1e-10));
   BOOST_CHECK(! is_equal( 0.000000001,  dMin,        1e-10));
   BOOST_CHECK(! is_equal( dMin,         0.000000001, 1e-10));
   BOOST_CHECK(! is_equal(-dMin,         0.000000001, 1e-10));
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------
