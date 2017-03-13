/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_fraction

#include "ecbuild/boost_test_framework.h"

#include "eckit/types/Fraction.h"

#include "eckit/testing/Setup.h"


using namespace eckit;
using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE( Setup );

BOOST_AUTO_TEST_SUITE( test_eckit_fraction )

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_fraction )
{
    // 0

    BOOST_CHECK_EQUAL( Fraction(0, 1), Fraction() );

    BOOST_CHECK_THROW( Fraction(0, 0), std::exception ); // this prints a backtrace wi Assertion failed

    // negative number

    BOOST_CHECK_EQUAL( Fraction(-1, 2), Fraction(3, -6) );

    // decimals

    BOOST_CHECK_EQUAL( Fraction(0.16), Fraction(16, 100) );
    BOOST_CHECK_EQUAL( Fraction(0.1616), Fraction(1616, 10000) );

    // 5 / 7

    BOOST_CHECK( Fraction(0.714285) != Fraction(5, 7) );

    BOOST_CHECK( Fraction(0.7142857142) != Fraction(5, 7) );

    BOOST_CHECK_EQUAL( Fraction(0.71428571428),  Fraction(5, 7) );
    BOOST_CHECK_EQUAL( Fraction(0.714285714285), Fraction(5, 7) );
    BOOST_CHECK_EQUAL( Fraction(0.714285714286), Fraction(5, 7) );

    BOOST_CHECK_EQUAL( Fraction(0.714285714285714285), Fraction(5, 7) );

    // 1 / 6

    BOOST_CHECK( Fraction(0.166) != Fraction(1, 6) );
    BOOST_CHECK( Fraction(0.1666) != Fraction(1, 6) );
    BOOST_CHECK( Fraction(0.16666) != Fraction(1, 6) );
    BOOST_CHECK( Fraction(0.166666) != Fraction(1, 6) );
    BOOST_CHECK( Fraction(0.1666666) != Fraction(1, 6) );
    BOOST_CHECK( Fraction(0.16666666) != Fraction(1, 6) );
    BOOST_CHECK( Fraction(0.166666666) != Fraction(1, 6) );

    BOOST_CHECK_EQUAL( Fraction(0.16666666666), Fraction(1, 6) );
    BOOST_CHECK_EQUAL( Fraction(0.166666666666), Fraction(1, 6) );
    BOOST_CHECK_EQUAL( Fraction(0.1666666666666), Fraction(1, 6) );
    BOOST_CHECK_EQUAL( Fraction(0.16666666666666), Fraction(1, 6) );
    BOOST_CHECK_EQUAL( Fraction(0.166666666666666), Fraction(1, 6) );
    BOOST_CHECK_EQUAL( Fraction(0.1666666666666666), Fraction(1, 6) );

    // 1 / 3

    BOOST_CHECK_EQUAL( Fraction(0.3333333333), Fraction(1, 3) );

    BOOST_CHECK_EQUAL( Fraction(0.3333333333333333), Fraction(1, 3) );

    // 1 / 1

    BOOST_CHECK_EQUAL( Fraction(0.9999999999999999), Fraction(1, 1) );
    BOOST_CHECK_EQUAL( Fraction(0.9999999999999999), Fraction(10, 10) );

    // 7 / 10..

    BOOST_CHECK_EQUAL( Fraction(0.7), Fraction(7, 10) );
    BOOST_CHECK_EQUAL( Fraction(0.07), Fraction(7, 100) );
    BOOST_CHECK_EQUAL( Fraction(0.0000007), Fraction(7, 10000000) );

    // operations

    BOOST_CHECK_EQUAL( Fraction(1, 3) + Fraction(2, 3), Fraction(10, 10) );
    BOOST_CHECK_EQUAL( Fraction(1, 3) - Fraction(2, 6), Fraction(0, 10) );

    BOOST_CHECK_EQUAL( Fraction(1, 3) * 3, Fraction(1) );

    BOOST_CHECK_EQUAL( -Fraction(1, 3), Fraction(1, -3) );

    BOOST_CHECK_EQUAL( 2 * Fraction(1, 3) , Fraction(2, 3) );

    Fraction a(1, 3);
    Fraction b(3, 28);
    BOOST_CHECK_EQUAL(a + b, Fraction(37, 84));
    BOOST_CHECK_EQUAL(a - b, Fraction(19, 84));
    BOOST_CHECK_EQUAL(a * b, Fraction(1, 28));
    BOOST_CHECK_EQUAL(a / b, Fraction(28, 9));

    BOOST_CHECK_EQUAL(a > b, true);
    BOOST_CHECK_EQUAL(a != b, true);
    BOOST_CHECK_EQUAL(a < b, false);
    BOOST_CHECK_EQUAL(a == b, false);

    BOOST_CHECK_EQUAL(Fraction("1/3"), Fraction(1, 3));
    BOOST_CHECK_EQUAL(double(Fraction("1/3")), 1.0 / 3.0);

    BOOST_CHECK_EQUAL(Fraction("1"), Fraction(1));
    BOOST_CHECK_EQUAL(Fraction("1.2"), Fraction(12, 10));
    BOOST_CHECK_EQUAL(Fraction("1e-6"), Fraction(1, 1000000));
    BOOST_CHECK_EQUAL(Fraction("1e+6"), Fraction(1000000, 1));
    BOOST_CHECK_EQUAL(Fraction("1.2e+6"), Fraction(1200000, 1));

    {
        Fraction west(-12), east(1.2), increment(1.2);

        Fraction f(west);
        while (f < east) {
            f += increment;
        }
        BOOST_CHECK_EQUAL(f, east);
    }

    {
        Fraction west("-77"), east("7"), increment("0.7");

        Fraction f(west);
        while (f < east) {
            f += increment;
        }
        BOOST_CHECK_EQUAL(f, east);
    }


}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
