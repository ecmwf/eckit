// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Regex.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Escape special regex chars in string") {
    EXPECT(Regex::escape(".^$*+-?()[]{}\\|") == "\\.\\^\\$\\*\\+\\-\\?\\(\\)\\[\\]\\{\\}\\\\\\|");
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
