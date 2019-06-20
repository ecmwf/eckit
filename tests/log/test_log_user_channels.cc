/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/log/Channel.h"
#include "eckit/log/ColouringTarget.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/runtime/Main.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_user_log_channel_registration") {

    // FIXME: re-enable this test

    //    EXPECT_NO_THROW( Log::registerChannel("mychannel", new Channel(new ColouringTarget(new
    //    OStreamTarget(std::cout), &Colour::green))) );

    //    EXPECT_NO_THROW( Log::channel("mychannel") << "TEST MY VERY OWN CHANNEL" << std::endl );
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
