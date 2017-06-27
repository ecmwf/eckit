/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <iostream>
#include <string>

#define BOOST_TEST_MODULE test_eckit_io
#include "eckit/eckit_config.h"

#include "ecbuild/boost_test_framework.h"

#include "eckit/io/CircularBuffer.h"
#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE( Setup );

BOOST_AUTO_TEST_SUITE( test_eckit_circularbuffer )

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_eckit_circularbuffer )
{
    CircularBuffer buffer(20);


    for (size_t j = 0; j < 100; ++j) {

        std::string lower;
        for (char c = 'a'; c <= 'z'; ++c) {
            buffer.write(&c, 1);
            lower += c;
        }

        std::string upper;
        for (char c = 'A'; c <= 'Z'; ++c) {
            buffer.write(&c, 1);
            upper += c;
        }

        char q[26];
        BOOST_CHECK_EQUAL(buffer.read(q, 26), 26);
        BOOST_CHECK_EQUAL(lower, std::string(q, q + 26));

        BOOST_CHECK_EQUAL(buffer.read(q, 26), 26);
        BOOST_CHECK_EQUAL(upper, std::string(q, q + 26));
    }


}


//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

// ----------------------------------------------------------------------------------------

