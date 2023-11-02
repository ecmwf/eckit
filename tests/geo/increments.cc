/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/Increments.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


using namespace geo;


CASE("checks") {
    EXPECT_NO_THROW(Increments(1, 2));
    EXPECT_NO_THROW(Increments(1, -2));
    EXPECT_NO_THROW(Increments(-1, 2));
    EXPECT_NO_THROW(Increments(-1, -2));

    EXPECT_THROWS(Increments(-1, 0));
    EXPECT_THROWS(Increments(0, -2));
    EXPECT_THROWS(Increments(0, 0));
    EXPECT_THROWS(Increments(0, 2));
    EXPECT_THROWS(Increments(1, 0));
}


CASE("assignment") {
    Increments a(10, 1);

    Increments b(20, 2);
    EXPECT_NOT_EQUAL(a.west_east, b.west_east);
    EXPECT_NOT_EQUAL(a, b);

    b = a;
    EXPECT_EQUAL(a.west_east, b.west_east);
    EXPECT_EQUAL(a, b);

    b.west_east = 30;
    EXPECT_EQUAL(b.west_east, 30);
    EXPECT_EQUAL(a.west_east, 10);

    Increments c(a);
    EXPECT_EQUAL(a.west_east, c.west_east);
    EXPECT_EQUAL(a, c);

    c.west_east = 40;
    EXPECT_EQUAL(c.west_east, 40);
    EXPECT_EQUAL(a.west_east, 10);

    auto d(std::move(a));
    EXPECT_EQUAL(d.west_east, 10);

    d.west_east = 50;
    EXPECT_EQUAL(d.west_east, 50);
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
