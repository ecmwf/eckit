/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_user_channels

#include <iostream>

#include "ecbuild/boost_test_framework.h"

#include "eckit/log/Channel.h"
#include "eckit/log/ColouringTarget.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/runtime/Main.h"

#include "eckit/testing/Setup.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

BOOST_GLOBAL_FIXTURE(Setup);

BOOST_AUTO_TEST_SUITE(test_eckit_user_log_channels)

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(test_eckit_user_log_channel_registration) {

//    BOOST_CHECK_NO_THROW( Log::registerChannel("mychannel", new Channel(new ColouringTarget(new OStreamTarget(std::cout), &Colour::green))) );

//    BOOST_CHECK_NO_THROW( Log::channel("mychannel") << "TEST MY VERY OWN CHANNEL" << std::endl );
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
