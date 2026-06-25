/*
 * (C) Copyright 2026- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdlib>

#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpDir.h"
#include "eckit/runtime/Main.h"
#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Main::rundir selects run when present and falls back to etc") {
    const char* expected = ::getenv("ECKIT_TEST_RUNDIR_EXPECT");
    EXPECT(expected != nullptr);

    const PathName home(Main::instance().home());
    const std::string expectedName(expected);

    EXPECT(expectedName == "run" || expectedName == "etc");

    if (expectedName == "run") {
        (home / "run").mkdir();
    }

    EXPECT_EQUAL(Main::instance().rundir(), home / expectedName);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    TmpDir home;
    SetEnv env("_TEST_ECKIT_HOME", home.asString());
    Main::initialise(argc, argv, "_TEST_ECKIT_HOME");
    return run_tests(argc, argv, false);
}
