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
#include "eckit/utils/TypeToString.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("typeToString<std::string>()") {
    EXPECT_EQUAL(typeToString<std::string>(), "std::string");
}
CASE("typeToString<const std::string>()") {
    EXPECT_EQUAL(typeToString<const std::string>(), "const std::string");
}
CASE("typeToString<const std::string&>()") {
    EXPECT_EQUAL(typeToString<const std::string&>(), "const std::string&");
}

CASE("typeToString<int>()") {
    EXPECT_EQUAL(typeToString<int>(), "int");
}
CASE("typeToString<const int>()") {
    EXPECT_EQUAL(typeToString<const int>(), "const int");
}
CASE("typeToString<const int&>()") {
    EXPECT_EQUAL(typeToString<const int&>(), "const int&");
}

CASE("typeToString<double>()") {
    EXPECT_EQUAL(typeToString<double>(), "double");
}
CASE("typeToString<const double>()") {
    EXPECT_EQUAL(typeToString<const double>(), "const double");
}
CASE("typeToString<const double&>()") {
    EXPECT_EQUAL(typeToString<const double&>(), "const double&");
}


namespace myspace {
template <typename X>
struct MyType;
}  // namespace myspace

}  // namespace eckit::test

namespace eckit {
template <typename X>
struct TypeToString<eckit::test::myspace::MyType<X>> {
    std::string operator()() const {
        return std::string("eckit::test::myspace::MyType<") + eckit::typeToString<X>() + std::string(">");
    }
};
}  // namespace eckit

namespace eckit::test {

CASE("typeToString<eckit::test::myspace::MyType<int>>()") {
    EXPECT_EQUAL(typeToString<eckit::test::myspace::MyType<int>>(), "eckit::test::myspace::MyType<int>");
}
CASE("typeToString<eckit::test::myspace::MyType<std::string>>()") {
    EXPECT_EQUAL(typeToString<eckit::test::myspace::MyType<std::string>>(),
                 "eckit::test::myspace::MyType<std::string>");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
