// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/Log.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_default") {
    /* log before context build */

    Log::info() << "logging before calling Context" << std::endl;

    /* setting context another time */


    Log::info() << "logging after resetting behavior" << std::endl;
    Log::debug() << "logging after resetting behavior" << std::endl;
    Log::warning() << "logging after resetting behavior" << std::endl;
    Log::error() << "logging after resetting behavior" << std::endl;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
