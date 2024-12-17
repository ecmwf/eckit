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


namespace eckit::geo::test {


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
    EXPECT_NOT_EQUAL(a.dx, b.dx);
    EXPECT_NOT_EQUAL(a, b);

    b = a;
    EXPECT_EQUAL(a.dx, b.dx);
    EXPECT_EQUAL(a, b);

    b = {30, b.dy};
    EXPECT_EQUAL(b.dx, 30);
    EXPECT_EQUAL(a.dx, 10);

    Increments c(a);
    EXPECT_EQUAL(a.dx, c.dx);
    EXPECT_EQUAL(a, c);

    c = {40, c.dy};
    EXPECT_EQUAL(c.dx, 40);
    EXPECT_EQUAL(a.dx, 10);

    auto d(std::move(a));
    EXPECT_EQUAL(d.dx, 10);

    d = {50, d.dy};
    EXPECT_EQUAL(d.dx, 50);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
