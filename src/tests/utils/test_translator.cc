/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/eckit_config.h"

#define BOOST_TEST_MODULE TestTranslator

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/parser/StringTools.h"
#include "eckit/utils/Translator.h"

using namespace std;
using namespace eckit;

BOOST_AUTO_TEST_SUITE( TestTranslator )

BOOST_AUTO_TEST_CASE( test_translator_string_double )
{
    double half = 0.5;
    double zero = 0.0;
    double hund = 100.0;
    double ten  = 10.;

    Translator<string,double> t;

    BOOST_CHECK( half == t( "0.5" ) );

    BOOST_CHECK( hund == t( "1e2"  ) );
    BOOST_CHECK( hund == t( "1E2"  ) );
    BOOST_CHECK( hund == t( "1.e2"  ) );
    BOOST_CHECK( hund == t( "1.E2"  ) );
    BOOST_CHECK( hund == t( "1.0e2"  ) );
    BOOST_CHECK( hund == t( "1.0E2"  ) );
    BOOST_CHECK( hund == t( "1e002"  ) );
    BOOST_CHECK( hund == t( "1E002"  ) );
    BOOST_CHECK( hund == t( "1.0e002"  ) );
    BOOST_CHECK( hund == t( "1.0E002"  ) );

    BOOST_CHECK( zero == t( "0"       ) );
    BOOST_CHECK( zero == t( "0."      ) );
    BOOST_CHECK( zero == t( "0.0"     ) );
    BOOST_CHECK( zero == t( "0.00"    ) );
    BOOST_CHECK( zero == t( "0.0e0"   ) );
    BOOST_CHECK( zero == t( "0.0e-0"  ) );
    BOOST_CHECK( zero == t( "0.0e+0"  ) );
    BOOST_CHECK( zero == t( "+0"      ) );
    BOOST_CHECK( zero == t( "+0."     ) );
    BOOST_CHECK( zero == t( "+0.0"    ) );
    BOOST_CHECK( zero == t( "+0.00"   ) );
    BOOST_CHECK( zero == t( "+0.0e0"  ) );
    BOOST_CHECK( zero == t( "+0.0e-0" ) );
    BOOST_CHECK( zero == t( "+0.0e+0" ) );
    BOOST_CHECK( zero == t( "-0"      ) );
    BOOST_CHECK( zero == t( "-0."     ) );
    BOOST_CHECK( zero == t( "-0.0"    ) );
    BOOST_CHECK( zero == t( "-0.00"   ) );
    BOOST_CHECK( zero == t( "-0.0e0"  ) );
    BOOST_CHECK( zero == t( "-0.0e-0" ) );
    BOOST_CHECK( zero == t( "-0.0e+0" ) );

    /// weird cases that actually pass

    BOOST_CHECK_NO_THROW( t( "inf" ) ); // inf is acceptable, case insensitive
    BOOST_CHECK_NO_THROW( t( "INF" ) ); // INF is acceptable, case insensitive
    BOOST_CHECK_NO_THROW( t( "infinity" ) ); // infinity is acceptable, case insensitive
    BOOST_CHECK_NO_THROW( t( "INFINITY" ) ); // INFINITY is acceptable, case insensitive

    BOOST_CHECK_NO_THROW( t( "nan" ) ); // nan is acceptable, case insensitive
    BOOST_CHECK_NO_THROW( t( "NAN" ) ); // NAN is acceptable, case insensitive

    BOOST_CHECK( zero == t( "0x0" ) );   // hexadecimal is acceptable
    BOOST_CHECK( ten  == t( "0xA" ) );   // hexadecimal is acceptable

    /// these should fail ...

    BOOST_CHECK_THROW( t( "" ), BadParameter ); // empty string

    BOOST_CHECK_THROW( t("0.5 "), BadParameter ); // no spaces accepted -- we are being extra strict
    BOOST_CHECK_THROW( t(" 0.5"), BadParameter ); // no spaces accepted -- we are being extra strict
    BOOST_CHECK_THROW( t(" 0.5   "), BadParameter ); // no spaces accepted -- we are being extra strict

    BOOST_CHECK_THROW( t( "1e+10000" ), BadParameter ); // overflow,  max ~ 1.79769e+308
    BOOST_CHECK_THROW( t( "1e-10000" ), BadParameter ); // underflow, min ~ 2.22507e-308

    BOOST_CHECK_THROW( t( "0.5a"   ), BadParameter );
    BOOST_CHECK_THROW( t( "foobar"  ), BadParameter );
    BOOST_CHECK_THROW( t( "foo555bar" ), BadParameter );
}

BOOST_AUTO_TEST_SUITE_END()
