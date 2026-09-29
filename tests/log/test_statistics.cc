// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Metin Cakircali
/// @date   May 2026

#include <regex>
#include <sstream>
#include <string>

#include "eckit/log/Statistics.h"
#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("reportCount produces exact formatted output") {
    std::ostringstream oss;
    Statistics::reportCount(oss, "Count", 42);
    EXPECT_EQUAL(oss.str(), std::string("Count                           : 42\n"));
}

CASE("reportCount with zero and always=false produces no output") {
    std::ostringstream oss;
    Statistics::reportCount(oss, "Count", 0, "", false);
    EXPECT_EQUAL(oss.str(), std::string(""));
}

CASE("reportCount with zero and always=true produces exact output") {
    std::ostringstream oss;
    Statistics::reportCount(oss, "Count", 0, "", true);
    EXPECT_EQUAL(oss.str(), std::string("Count                           : 0\n"));
}

CASE("reportCount with indent") {
    std::ostringstream oss;
    Statistics::reportCount(oss, "Count", 7, "  ");
    EXPECT_EQUAL(oss.str(), std::string("  Count                           : 7\n"));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("reportBytes produces exact formatted output") {
    std::ostringstream oss;
    Statistics::reportBytes(oss, "Data size", 1048576ULL);
    EXPECT_EQUAL(oss.str(), std::string("Data size                       : 1,048,576 (1 Mbyte)\n"));
}

CASE("reportBytes with zero and always=false produces no output") {
    std::ostringstream oss;
    Statistics::reportBytes(oss, "Data size", 0ULL, "", false);
    EXPECT_EQUAL(oss.str(), std::string(""));
}

//----------------------------------------------------------------------------------------------------------------------

CASE("reportTime with Timing produces exact formatted output") {
    std::ostringstream oss;
    Timing t{2.0, 1.0, 3};
    Statistics::reportTime(oss, "Elapsed", t);
    EXPECT_EQUAL(oss.str(), std::string("Elapsed                         : 2 seconds (1 second CPU). Updates: 3\n"));
}

CASE("reportTime with double produces exact formatted output") {
    std::ostringstream oss;
    Statistics::reportTime(oss, "Wall time", 60.0);
    EXPECT_EQUAL(oss.str(), std::string("Wall time                       : 1 minute\n"));
}

CASE("reportTime with zero Timing and always=false produces no output") {
    std::ostringstream oss;
    Timing t;
    Statistics::reportTime(oss, "Elapsed", t, "", false);
    EXPECT_EQUAL(oss.str(), std::string(""));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
