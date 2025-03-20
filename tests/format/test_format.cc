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

CASE("Can use eckit_format macro") {
    EXPECT_EQUAL(eckit_format("Hello {} {}", 1, 2), std::string("Hello 1 2"));
    EXPECT_EQUAL(eckit_format(std::string_view("Hello {} {}"), 1, 2), std::string("Hello 1 2"));
}

CASE("Can use eckit::str_format") {
    EXPECT_EQUAL(eckit::str_format("Hello {} {}", 1, 2), std::string("Hello 1 2"));
    EXPECT_EQUAL(eckit::str_format(std::string_view("Hello {} {}"), 1, 2), std::string("Hello 1 2"));
    EXPECT_EQUAL(eckit::str_format(std::string("Hello {} {}"), 1, 2), std::string("Hello 1 2"));
    std::string str("Hello {} {}");
    std::string_view str_v(str);
    EXPECT_EQUAL(eckit::str_format(str_v, 1, 2), std::string("Hello 1 2"));
}

CASE("eckit::str_format throws on format erors") {
    EXPECT_THROWS_AS(eckit::str_format("Hello {} {} {}", 1, 2), fmt::format_error);
    EXPECT_THROWS_AS(eckit::str_format(std::string("Hello {} {} {}"), 1, 2), fmt::format_error);
    EXPECT_THROWS_AS(eckit::str_format(std::string_view("Hello {} {} {}"), 1, 2), fmt::format_error);
    EXPECT_THROWS_AS(eckit::str_format("Hello {} {} {", 1, 2), fmt::format_error);
    EXPECT_THROWS_AS(eckit::str_format(std::string("Hello {} {} {"), 1, 2), fmt::format_error);
    EXPECT_THROWS_AS(eckit::str_format(std::string_view("Hello {} {} {"), 1, 2), fmt::format_error);
    EXPECT_THROWS_AS(eckit::str_format("Hello {:p} {} {}", 1, 2), fmt::format_error);
    EXPECT_THROWS_AS(eckit::str_format(std::string("Hello {:p} {} {}"), 1, 2), fmt::format_error);
    EXPECT_THROWS_AS(eckit::str_format(std::string_view("Hello {:p} {} {}"), 1, 2), fmt::format_error);
}

struct MyType {
    int x{5};
};
std::ostream& operator<<(std::ostream& out, const MyType& mt) {
    return out << "MyType{" << mt.x << "}";
}
ENABLE_FORMAT(MyType);

CASE("ENABLE_FORMAT works") {
    EXPECT_EQUAL(eckit::str_format("Hello {}", MyType{}), std::string("Hello MyType{5}"));
}

ENABLE_FORMAT(eckit::CodeLocation);
CASE("ENABLE_FORMAT CodeLocation") {
    // This is just an example
    std::cout << eckit::str_format("{}", Here());
}

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
