/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/SemanticVersion.h"
#include "eckit/types/Types.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Empty versions") {

    SemanticVersion v;
    EXPECT(std::string(v) == "0.0.0");

    SemanticVersion v0("0.0.0");
    SemanticVersion v1("1.0.0");
    SemanticVersion v2("0.1.0");
    SemanticVersion v3("0.0.1");

    EXPECT(v == v0);
    EXPECT(v < v1);
    EXPECT(v < v2);
    EXPECT(v < v3);
}

CASE("Construct with integers") {
    EXPECT(SemanticVersion() == SemanticVersion(0, 0, 0));
    EXPECT(SemanticVersion("0.0.0") == SemanticVersion(0, 0, 0));
    EXPECT(SemanticVersion("27.125.22") == SemanticVersion(27, 125, 22));
    EXPECT(SemanticVersion("2.1.3") != SemanticVersion(27, 125, 22));
}

CASE("Failed construction") {
    EXPECT_THROWS_AS(SemanticVersion("1"), eckit::BadValue);
    EXPECT_THROWS_AS(SemanticVersion("2.3"), eckit::BadValue);
    EXPECT_THROWS_AS(SemanticVersion("1.2.3.4"), eckit::BadValue);
    EXPECT_THROWS_AS(SemanticVersion("1.2-4"), eckit::BadValue);

    EXPECT_THROWS_AS(SemanticVersion("1.2.4-r1"), eckit::BadValue);
    EXPECT_THROWS_AS(SemanticVersion("1.-2.4"), eckit::BadValue);
    EXPECT_THROWS_AS(SemanticVersion("1.2.456abc"), eckit::BadValue);
    EXPECT_THROWS_AS(SemanticVersion("1.aaa.56"), eckit::BadValue);
    EXPECT_THROWS_AS(SemanticVersion("v1.2.4"), eckit::BadValue);
    EXPECT_THROWS_AS(SemanticVersion("x.y.z"), eckit::BadValue);
    EXPECT_THROWS_AS(SemanticVersion("1.2.999999999999999999999999999999"), eckit::OutOfRange);
}

CASE("Version comparison") {
    std::ostringstream ss;
    ss << SemanticVersion("27.125.22");
    EXPECT(ss.str() == "27.125.22");
}

CASE("Version comparison") {

    SemanticVersion v1("1.0.0");
    SemanticVersion v2("0.1.0");
    SemanticVersion v3("0.0.1");

    EXPECT(std::string(v1) == "1.0.0");
    EXPECT(std::string(v2) == "0.1.0");
    EXPECT(std::string(v3) == "0.0.1");

    EXPECT(v1 > v2);
    EXPECT(v1 > v3);

    EXPECT(v2 < v1);
    EXPECT(v2 > v3);

    EXPECT(v3 < v1);
    EXPECT(v3 < v2);
}

CASE("Multidigit versions") {

    SemanticVersion v("27.125.22");

    EXPECT(std::string(v) == "27.125.22");

    EXPECT(v == SemanticVersion("27.125.22"));
    EXPECT(v != SemanticVersion("27.125.33"));
    EXPECT(v != SemanticVersion("27.25.22"));
    EXPECT(v != SemanticVersion("7.125.22"));

    EXPECT(v > SemanticVersion("27.125.11"));
    EXPECT(v > SemanticVersion("27.1.33"));
    EXPECT(v > SemanticVersion("2.125.2"));

    EXPECT(v < SemanticVersion("27.125.33"));
    EXPECT(v < SemanticVersion("27.777.33"));
    EXPECT(v < SemanticVersion("42.125.22"));

    EXPECT(v >= SemanticVersion("27.125.22"));
    EXPECT(v >= SemanticVersion("27.125.12"));
    EXPECT(v <= SemanticVersion("27.125.22"));
    EXPECT(v <= SemanticVersion("27.125.33"));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
