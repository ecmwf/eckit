// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/spec/Layered.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::spec::test {


CASE("Spec <- Layered") {
    int one                              = 1;
    double two                           = 2.;
    std::vector<bool> expected_bool_list = {true, false, true};

    spec::Custom a({{"foo", one}, {"bar", two}, {"bool_list", expected_bool_list}});
    ASSERT(a.has("foo"));
    ASSERT(a.has("bar"));
    ASSERT(a.has("bool_list"));

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

    std::vector<bool> bool_list;
    EXPECT(b.get("bool_list", bool_list));
    EXPECT(bool_list == expected_bool_list);

    b.hide("bool_list");
    EXPECT_NOT(b.get("bool_list", bool_list));
}


}  // namespace eckit::spec::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
