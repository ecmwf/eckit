// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/filesystem/PathName.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Unknown path type fails") {
    EXPECT_THROWS_AS(PathName("unknown://a/path/component"), SeriousBug);
}

/* CASE("Creation of marsfs paths") {
    PathName m("marsfs://nodexxx/a/path/component");

    EXPECT(m.node() == "nodexxx");
    EXPECT(m.path() == "/a/path/component");
}*/

CASE("Creation of local paths") {

    PathName a("local:///an/absolute/path");
    PathName b("/an/absolute/path");

    PathName c("local://a/relative/path");
    PathName d("a/relative/path");

    EXPECT(a == b);
    EXPECT(c == d);
    EXPECT(a != c);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
