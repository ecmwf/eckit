// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/order/HEALPix.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Layered.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("canonical") {
    for (const auto& gridSpec : std::vector<std::string>{
             R"({"area":[73,-27,33,45],"grid":[4,4],"reference":[1,1]})",
         }) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(gridSpec));

        EXPECT(grid);
        EXPECT(grid->spec_str() == gridSpec);
    }
}


CASE("canonical (global grids)") {
    struct test_t {
        std::string gridspec;
        std::string canonical;
        size_t size;
    };

    static const auto bbox_global = area::BoundingBox::bounding_box_default().spec_str();

    for (const auto& test : {
             test_t{"{grid: 10/10}", R"({"grid":[10,10]})", 684},
             {"{grid: [20, 10]}", R"({"grid":[20,10]})", 342},
             {"{pl: [20, 24, 24, 20]}", R"({"grid":"O2"})", 88},
             {"{grid: o8}", R"({"grid":"O8"})", 544},
             {"{grid: hr2}", R"({"grid":"H2"})", 48},
             {"{grid: h2n}", R"({"grid":"H2","order":"nested"})", 48},
             {"{grid: o96}", R"({"grid":"O96"})", 40320},
         }) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.gridspec));

        EXPECT_EQUAL(grid->spec_str(), test.canonical);
        EXPECT_EQUAL(grid->size(), test.size);
        EXPECT_EQUAL(grid->boundingBox().spec_str(), bbox_global);
    }
}


CASE("routings") {
    for (const auto* gridspec : {"{grid: [1,1]}", "{grid: 1/1}"}) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(gridspec));

        EXPECT_EQUAL(grid->spec_str(), R"({"grid":[1,1]})");
        EXPECT(grid->shape() == std::vector<size_t>({181, 360}));
    }

    for (const auto* gridspec : {
             "{grid: 0.05/0.05, area: [89.975,-179.975,-89.975,179.975]}",
             "{grid: [0.05, 0.05], area: 89.975/-179.975/-89.975/179.975}",
         }) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(gridspec));

        EXPECT_EQUAL(grid->spec_str(),
                     R"({"area":[90,-179.975,-90,180.025],"grid":[0.05,0.05],"reference":[0.025,0.025]})");
        EXPECT(grid->shape() == std::vector<size_t>({3600, 7200}));
    }
}


CASE("type (rotated regional)") {
    std::unique_ptr<const Grid> grid(
        GridFactory::make_from_string(R"({"area":[3.36,-6.82,-4.42,4.8],"grid":[0.02,0.02],"order":"i+j+",)"
                                      R"("projection":{"south_pole":[10,-43],"type":"rotation"}})"));

    EXPECT_EQUAL(grid->type(), "regular_ll");
}


CASE("spec round trip (same points)") {
    std::vector<std::string> gridspecs{
        "{grid: [2, 2], area: [60, -10, 30, 40]}",
        R"({"area":[3.4,-6.8,-4.4,4.8],"grid":[0.2,0.2],"order":"i+j+","projection":{"south_pole":[10,-43],"type":"rotation"}})",
        "{grid: F16}",
        "{grid: F16, area: [60, -10, 30, 40]}",
        "{grid: O16}",
        "{grid: N32}",
        "{pl: [20, 24, 24, 20]}",
        "{grid: O16, area: [60, -10, 30, 40]}",
        "{grid: H4}",
        "{grid: H4, order: nested}",
    };

    for (const auto* grid : {"grid: 10/10", "grid: 10/10, area: [60, -10, 30, 40]", "grid: F8"}) {
        for (const auto* order : {"i+j-", "i+j+", "i-j-", "i-j+", "j-i+", "j+i+", "j-i-", "j+i-"}) {
            gridspecs.emplace_back("{" + std::string(grid) + ", order: " + order + "}");
        }
    }

    for (const auto& gridspec : gridspecs) {
        SECTION(gridspec) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string(gridspec));
            std::unique_ptr<const Grid> same(GridFactory::make_from_string(grid->spec_str()));

            const auto [lats, lons]           = grid->to_latlons();
            const auto [same_lats, same_lons] = same->to_latlons();
            EXPECT_EQUAL(same_lats.size(), lats.size());

            for (size_t i = 0; i < lats.size(); ++i) {
                EXPECT(points_equal(PointLonLat{lons[i], lats[i]}, PointLonLat{same_lons[i], same_lats[i]}));
            }
        }
    }
}


CASE("user -> type") {
    using v = std::vector<double>;

    static const std::string BAD;
    ASSERT(BAD.empty());

    static std::pair<spec::Custom::container_type, std::string> tests[]{
        {{{"N", 2}}, "reduced_gg"},
        {{{"area", v{90, -180, -90, 180}}, {"grid", v{2, 2}}}, "regular_ll"},
        {{{"area", v{90, -180, -90, 180}}}, BAD},
        {{{"grid", "B48"}}, BAD},
        {{{"grid", "F48"}}, "regular_gg"},
        {{{"grid", "N48"}}, "reduced_gg"},
        {{{"grid", "O48"}}, "reduced_gg"},
        {{{"grid", 48}}, BAD},
        {{{"grid", v{2, 2}}}, "regular_ll"},
        {{{"grid", v{2, 2}}}, "regular_ll"},
        {{{"grid", "48"}}, BAD},
        {{{"grid", "F048"}}, BAD},
        {{{"grid", "N"}}, BAD},
        {{{"grid", "N048"}}, BAD},
        {{{"grid", "N48"}}, "reduced_gg"},
        {{{"grid", "O048"}}, BAD},
        {{{"grid", "O48"}}, "reduced_gg"},
        {{{"type", "reduced_gg"}}, ""},
        {{{"grid", 2}}, ""},
        {{{"grid", 12}}, ""},
        {{{"type", "regular_gg"}, {"grid", "48"}}, BAD},
        {{{"type", "regular_gg"}, {"grid", "F048"}}, BAD},
        {{{"type", "regular_gg"}, {"grid", "F48"}}, "regular_gg"},
        {{{"type", "regular_gg"}, {"grid", "N48"}}, "reduced_gg"},
        {{{"type", "regular_gg"}, {"grid", "O48"}}, "reduced_gg"},
        {{{"type", "regular_gg"}, {"grid", "a"}}, BAD},
        {{{"type", "regular_gg"}, {"grid", 48}}, BAD},
        {{{"type", "regular_ll"}, {"area", v{90, -180, -90, 180}}}, BAD},
        {{{"type", "regular_ll"}, {"grid", "F48"}}, BAD},
        {{{"type", "regular_ll"}, {"grid", "a"}}, BAD},
        {{{"type", "regular_ll"}, {"grid", std::vector<std::string>{"a", "b"}}}, BAD},
        {{{"type", "regular_ll"}, {"grid", v{1, 2, 3}}}, BAD},
        {{{"type", "regular_ll"}, {"grid", v{1, 2}}}, "regular_ll"},
        {{{"type", "regular_ll"}, {"grid", v{1}}}, BAD},

        {{{"type", "mercator"},
          {"area", v{31.173058, 262.036499, 14.736453, 284.975281}},
          {"grid", v{45000.0, 45000.0}},
          {"shape", std::vector<size_t>{56, 44}},
          {"lad", 14.0},
          {"orientation", 0.0}},
         BAD},
    };

    for (const auto& [user, ref] : tests) {
        spec::Custom userspec(user);

        try {
            std::unique_ptr<const Grid::Spec> spec(GridFactory::make_spec(userspec));
            EXPECT(spec);

            std::unique_ptr<const Grid> grid(GridFactory::build(*spec));
            EXPECT(grid);
        }
        catch (const exception::GridError& e) {
            EXPECT(ref == BAD);
        }
        catch (const exception::SpecError& e) {
            EXPECT(ref == BAD);
        }
        catch (const BadParameter& e) {
            EXPECT(ref == BAD);
        }
    }
}


CASE("grid: name -> spec -> grid: name") {
// FIXME
#if 0
    for (const std::string& name : {"LAEA-EFAS-5km", "SMUFF-OPERA-2km"}) {
        std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom({{"grid", name}})));
        EXPECT(grid);

        auto gridspec = grid->spec_str();
        EXPECT(gridspec == R"({"grid":")" + name + R"("})");
    }
#endif
}


CASE("grid: reduced_gg") {
    SECTION("O16, N16") {
        std::unique_ptr<const Grid> o16(GridFactory::build(spec::Custom({{"grid", "o16"}})));

        EXPECT(o16->spec_str() == R"({"grid":"O16"})");

        std::unique_ptr<const Grid> n16(GridFactory::build(spec::Custom({{"grid", "n16"}})));

        EXPECT(n16->spec_str() == R"({"grid":"N16"})");

        std::unique_ptr<const Grid> known_pl_1(GridFactory::build(
            spec::Custom({{"pl", pl_type{20, 27, 32, 40, 45, 48, 60, 60, 64, 64, 64, 64, 64, 64, 64, 64,
                                         64, 64, 64, 64, 64, 64, 64, 64, 60, 60, 48, 45, 40, 32, 27, 20}}})));

        EXPECT(known_pl_1->spec_str() == R"({"grid":"N16"})");

        std::unique_ptr<const Grid> known_pl_2(
            GridFactory::build(spec::Custom({{"pl", pl_type{20, 24, 28, 32, 32, 28, 24, 20}}})));

        EXPECT(known_pl_2->spec_str() == R"({"grid":"O4"})");

        std::unique_ptr<const Grid> unknown_pl(
            GridFactory::build(spec::Custom({{"pl", pl_type{20, 24, 28, 32, 32, 28, 24, 99}}})));

        EXPECT(unknown_pl->spec_str() == R"({"grid":"N4","pl":[20,24,28,32,32,28,24,99]})");
    }


    SECTION("O96") {
        const auto spec     = R"({"grid":"O96", "area":[89.2842275325138,0,-89.2842275325138,359.1]})";
        const auto expected = R"({"grid":"O96"})";

        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(spec));
        EXPECT(grid->spec().str() == expected);
    }
}


CASE("grid: HEALPix") {
    std::unique_ptr<const Grid> h2(GridFactory::build(spec::Custom({{"grid", "h2"}})));

    EXPECT(h2->spec_str() == R"({"grid":"H2"})");

    std::unique_ptr<const Grid> h2n(GridFactory::build(spec::Custom({{"grid", "H2"}, {"order", "nested"}})));

    EXPECT(h2n->spec_str() == R"({"grid":"H2","order":"nested"})");
}


CASE("grid: regular_ll") {
    SECTION("arakawa c-grids") {
        spec::Custom spec({{"type", "arakawa_c_um"}, {"N", 96}});

        spec.set("arrangement", "T");
        auto N96_T = std::unique_ptr<const Grid>(GridFactory::build(spec))->spec_str();
        EXPECT(N96_T == R"({"grid":[1.875,1.25],"order":"i+j+","reference":[0.9375,0.625]})");

        spec.set("arrangement", "U");
        auto N96_U = std::unique_ptr<const Grid>(GridFactory::build(spec))->spec_str();
        EXPECT(N96_U == R"({"grid":[1.875,1.25],"order":"i+j+","reference":[0,0.625]})");

        spec.set("arrangement", "V");
        auto N96_V = std::unique_ptr<const Grid>(GridFactory::build(spec))->spec_str();
        EXPECT(N96_V == R"({"grid":[1.875,1.25],"order":"i+j+","reference":[0.9375,0]})");
    }
}


CASE("order") {
    struct test_t {
        const spec::Custom spec;
        const std::string order_default;
        const std::string order_nondefault;
        const std::string spec_str_order_default;
        const std::string spec_str_order_nondefault;
    };

    for (const auto& test : {
             test_t{{{"type", "regular_ll"}, {"grid", std::vector<double>{90, 90}}},
                    order::Scan::order_default(),
                    "i+j+",
                    R"({"grid":[90,90]})",
                    R"({"grid":[90,90],"order":"i+j+"})"},
             test_t{{{"type", "regular_gg"}, {"N", 2}},
                    order::Scan::order_default(),
                    "i+j+",
                    R"({"grid":"F2"})",
                    R"({"grid":"F2","order":"i+j+"})"},
             test_t{{{"grid", "H2"}},
                    order::HEALPix::order_default(),
                    order::HEALPix::NESTED,
                    R"({"grid":"H2"})",
                    R"({"grid":"H2","order":")" + order::HEALPix::NESTED + R"("})"},
         }) {
        ASSERT(test.order_default != test.order_nondefault);

        std::unique_ptr<const Grid> grid1(GridFactory::build(test.spec));

        EXPECT(grid1->order() == test.order_default);
        EXPECT(grid1->spec_str() == test.spec_str_order_default);

        spec::Layered spec(test.spec);
        spec.push_back(new spec::Custom({{"order", test.order_nondefault}}));

        std::unique_ptr<const Grid> grid2(GridFactory::build(spec));

        EXPECT(grid2->order() == test.order_nondefault);
        EXPECT(grid2->spec_str() == test.spec_str_order_nondefault);

        std::unique_ptr<const Grid> grid3(GridFactory::make_from_string(grid2->spec_str()));

        EXPECT(grid3->order() == grid2->order());
        EXPECT(grid3->spec_str() == grid2->spec_str());
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
