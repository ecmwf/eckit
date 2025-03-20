/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#include <eckit/testing/Test.h>

#include "eckit/format/Format.h"
#include "eckit/log/Channel.h"
#include "eckit/log/Log.h"
#include "eckit/log/OStreamTarget.h"

#include <sstream>

CASE("Format to output iterator") {
    {
        auto output = fmt::memory_buffer();
        eckit::str_format_to(std::back_inserter(output), "Hello {} {} {}", "through", "a", "stream");
        EXPECT_EQUAL(std::string(output.data(), output.size()), std::string("Hello through a stream"));
    }
    {
        std::vector<char> output;
        eckit::str_format_to(std::back_inserter(output), "Hello {} {} {}", "through", "a", "stream");
        EXPECT_EQUAL(std::string(output.data(), output.size()), std::string("Hello through a stream"));
    }
}

CASE("Format to streams (FMT_STRING)") {
    std::ostringstream oss;
    eckit_format_to(oss, "Hello {} {} {}", "through", "a", "stream");
    EXPECT_EQUAL(oss.str(), std::string("Hello through a stream"));
}
CASE("Format to streams (FMT_COMPLIE)") {
    std::ostringstream oss;
    eckit_format_to_cc(oss, "Hello {} {} {}", "through", "a", "stream");
    EXPECT_EQUAL(oss.str(), std::string("Hello through a stream"));
}
CASE("Format to streams (runtime)") {
    std::ostringstream oss;
    eckit::str_format_to(oss, "Hello {} {} {}", "through", "a", "stream");
    EXPECT_EQUAL(oss.str(), std::string("Hello through a stream"));
}


CASE("Format to eckit channel (FMT_STRING)") {
    std::ostringstream oss;
    eckit::Channel ch{new eckit::OStreamTarget{oss}};
    eckit_format_to(ch, "Hello {} {} {}", "through", "a", "stream");
    ch.flush();
    EXPECT_EQUAL(oss.str(), std::string("Hello through a stream"));
}
CASE("Format to eckit channel (FMT_COMPILE)") {
    std::ostringstream oss;
    eckit::Channel ch{new eckit::OStreamTarget{oss}};
    eckit_format_to_cc(ch, "Hello {} {} {}", "through", "a", "stream");
    ch.flush();
    EXPECT_EQUAL(oss.str(), std::string("Hello through a stream"));
}
CASE("Format to eckit channel (runtime)") {
    std::ostringstream oss;
    eckit::Channel ch{new eckit::OStreamTarget{oss}};
    eckit::str_format_to(ch, "Hello {} {} {}", "through", "a", "stream");
    ch.flush();
    EXPECT_EQUAL(oss.str(), std::string("Hello through a stream"));
}


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
