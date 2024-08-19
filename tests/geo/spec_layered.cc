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


#include "eckit/exception/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("Spec <- Layered") {
    int one    = 1;
    double two = 2.;

    spec::Custom a({{"foo", one}, {"bar", two}});
    ASSERT(a.has("foo"));
    ASSERT(a.has("bar"));

    spec::Layered b(a);

    ASSERT(b.has("foo"));
    ASSERT(b.has("bar"));

    b.hide("foo");
    EXPECT_NOT(b.has("foo"));

    b.unhide("foo");
    ASSERT(b.has("foo"));

    EXPECT(a.get_int("foo") == one);

    auto value = b.get_int("foo");
    EXPECT(value == one);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
