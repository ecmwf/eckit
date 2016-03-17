/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date March 2016

#define BOOST_TEST_MODULE test_eckit_config

#include "ecbuild/boost_test_framework.h"

#include "eckit/config/LocalConfiguration.h"
#include "eckit/parser/JSON.h"

using namespace eckit;

//----------------------------------------------------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE( test_eckit_config_localconfiguration )

BOOST_AUTO_TEST_CASE( test_eckit_localconfiguration_size ) {

    LocalConfiguration config;

    config.set("key1", long(1234));
    config.set("key1", long(4321));
    config.set("key2", "str val");
    config.set("key3", 123.45);
    config.set("key4", false);

    BOOST_CHECK_EQUAL(config.size(), 4);
}

BOOST_AUTO_TEST_CASE( test_eckit_localconfiguration_json ) {

    LocalConfiguration config;

    config.set("key1", long(1234));
    config.set("key1", long(4321));
    config.set("key2", "str val");
    config.set("key3", 123.45);
    config.set("key4", false);

    std::stringstream ss;
    eckit::JSON json(ss);

    config.json(json);

    BOOST_CHECK_EQUAL(ss.str(), "{\"key1\":4321,\"key2\":\"str val\",\"key3\":123.45,\"key4\":false}");
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
