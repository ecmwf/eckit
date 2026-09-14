// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
