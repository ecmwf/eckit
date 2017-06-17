/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_pathexpander

#include <string>

#include "ecbuild/boost_test_framework.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathExpander.h"

#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( TestPathExpander )

BOOST_AUTO_TEST_CASE( test_expands )
{
    std::string s = "{CWD}/tmp/foo";
    std::string r = std::string(::getenv("CURRENT_TEST_DIR")) + std::string("/tmp/foo");

    BOOST_CHECK_EQUAL( PathExpander::expand("CWD", s), r);
}

BOOST_AUTO_TEST_CASE( test_missing_keys )
{
    std::string s = "{FOO}/tmp/foo";

    BOOST_CHECK_THROW( PathExpander::expand("FOO", s), eckit::UserError );
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
