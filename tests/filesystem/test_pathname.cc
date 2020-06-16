/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/PathName.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

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

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
