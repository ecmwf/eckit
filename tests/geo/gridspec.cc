/*
 * (S) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <memory>
#include <utility>

#include "eckit/geo/Grid.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


#define EXPECT_APPROX(a, b, eps) EXPECT(::eckit::types::is_approximately_equal((a), (b), (eps)))

#define EXPECT_AREA(a, b, eps)                                                             \
    EXPECT((a).size() == 4 && (b).size() == 4 && EXPECT_APPROX((a)[0], (b)[0], (eps)) &&   \
           EXPECT_APPROX((a)[1], (b)[1], (eps)) && EXPECT_APPROX((a)[2], (b)[2], (eps)) && \
           EXPECT_APPROX((a)[3], (b)[3], (eps)))


namespace eckit::test {


using namespace geo;


using S = std::unique_ptr<const Spec>;
using G = std::unique_ptr<const Grid>;
using C = spec::Custom;
using v = std::vector<double>;


static const spec::Custom BAD;


static std::pair<C, C> cases[]{
    {C({{"N", 2}}), C({{"N", 2}, {"type", "reduced_gg"}})},
    {C({{"area", v{90, -180, -90, 180}}, {"grid", v{2, 2}}}), C({{"type", "regular_ll"}})},
    {C({{"area", v{90, -180, -90, 180}}}), BAD},
    {C({{"grid", "B48"}}), BAD},
    {C({{"grid", "F48"}}), C({{"type", "regular_gg"}})},
    {C({{"grid", "N48"}}), C({{"type", "reduced_gg"}})},
    {C({{"grid", "O48"}}), C({{"type", "reduced_gg"}})},
    {C({{"grid", 48}}), BAD},
    {C({{"grid", v{2, 2}}}), C({{"grid", v{2, 2}}, {"type", "regular_ll"}})},
    {C({{"type", "latlon"}, {"grid", v{2, 2}}}), BAD},
    {C({{"type", "reduced_gg"}, {"grid", "48"}}), BAD},
    {C({{"type", "reduced_gg"}, {"grid", "F048"}}), BAD},
    {C({{"type", "reduced_gg"}, {"grid", "N"}}), BAD},
    {C({{"type", "reduced_gg"}, {"grid", "N048"}}), BAD},
    {C({{"type", "reduced_gg"}, {"grid", "N48"}}), C({{"type", "reduced_gg"}})},
    {C({{"type", "reduced_gg"}, {"grid", "O048"}}), BAD},
    {C({{"type", "reduced_gg"}, {"grid", "O48"}}), C({{"type", "reduced_gg"}})},
    {C({{"type", "reduced_gg"}, {"grid", 48}}), BAD},
    {C({{"type", "reduced_gg"}}), BAD},
    {C({{"type", "reduced_latlon"}, {"grid", 2}}), BAD},
    {C({{"type", "reduced_ll"}, {"grid", 12}}), BAD},
    {C({{"type", "regular_gg"}, {"grid", "48"}}), BAD},
    {C({{"type", "regular_gg"}, {"grid", "F048"}}), BAD},
    {C({{"type", "regular_gg"}, {"grid", "F48"}}), C({{"type", "regular_gg"}})},
    {C({{"type", "regular_gg"}, {"grid", "N48"}}), BAD},
    {C({{"type", "regular_gg"}, {"grid", "O48"}}), BAD},
    {C({{"type", "regular_gg"}, {"grid", "a"}}), BAD},
    {C({{"type", "regular_gg"}, {"grid", 48}}), BAD},
    {C({{"type", "regular_ll"}, {"area", v{90, -180, -90, 180}}}), BAD},
    {C({{"type", "regular_ll"}, {"grid", "F48"}}), BAD},
    {C({{"type", "regular_ll"}, {"grid", "a"}}), BAD},
    {C({{"type", "regular_ll"}, {"grid", 48}}), BAD},
    {C({{"type", "regular_ll"}, {"grid", std::vector<std::string>{"a", "b"}}}), BAD},
    {C({{"type", "regular_ll"}, {"grid", v{1, 2, 3}}}), BAD},
    {C({{"type", "regular_ll"}, {"grid", v{1, 2}}}), C({{"type", "regular_ll"}})},
    {C({{"type", "regular_ll"}, {"grid", v{1}}}), BAD},

    {C({{"type", "mercator"},
        {"area", v{31.173058, 262.036499, 14.736453, 284.975281}},
        {"grid", v{45000.0, 45000.0}},
        {"lad", 14.0},
        {"nx", 56},
        {"ny", 44},
        {"orientation", 0.0}}),
     C()},
};


CASE("gridspec") {
    SECTION("user -> type") {
        for (const auto& [user, gridspec] : cases) {
            Log::info() << user << " -> " << gridspec << std::endl;

            S spec(GridFactory::spec(user));
            ASSERT(spec);

            if (!spec->has("type")) {
                EXPECT(gridspec.empty());
                continue;
            }

            if (gridspec.empty()) {
                EXPECT_THROWS(GridFactory::build(*spec));
            }
            else {
                EXPECT_NO_THROW(GridFactory::build(*spec));
            }
        }
    }


    SECTION("user -> grid -> gridspec") {
        for (const auto& [user, gridspec] : cases) {
            Log::info() << user << " -> " << gridspec << std::endl;

            if (gridspec.empty()) {
                EXPECT_THROWS(G(GridFactory::build(user)));
                continue;
            }

            // G grid(GridFactory::build(*user));
            // EXPECT(grid);
        }
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
