// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/grid/regular/RegularGaussian.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using grid::regular::RegularGaussian;


CASE("sizes") {
    struct test_t {
        explicit test_t(size_t N) : N(N), size(4 * N * 2 * N) {}
        size_t N;
        size_t size;
    } tests[]{test_t{2}, test_t{3}, test_t{64}};

    for (const auto& test : tests) {
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "f" + std::to_string(test.N)}})));
        std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"type", "regular_gg"}, {"N", test.N}})));
        RegularGaussian grid3(test.N);

        EXPECT(grid1->size() == test.size);
        EXPECT(grid2->size() == test.size);
        EXPECT(grid3.size() == test.size);
    }
}


CASE("points") {
    RegularGaussian grid(1);

    const std::vector<PointLonLat> ref{
        {0., 35.264389683},  {90., 35.264389683},  {180., 35.264389683},  {270., 35.264389683},  //
        {0., -35.264389683}, {90., -35.264389683}, {180., -35.264389683}, {270., -35.264389683},
    };

    auto points = grid.to_points();

    EXPECT(points.size() == grid.size());
    ASSERT(points.size() == ref.size());

    auto it = grid.begin();
    for (size_t i = 0; i < points.size(); ++i) {
        EXPECT(points_equal(ref[i], points[i]));
        EXPECT(points_equal(ref[i], *it));
        ++it;
    }
    EXPECT(it == grid.end());

    size_t i = 0;
    for (const auto& it : grid) {
        EXPECT(points_equal(ref[i++], it));
    }
    EXPECT(i == grid.size());
}


CASE("rotated") {
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{grid: F48, rotation: [30, 30]}"));

    EXPECT(grid->projection().type() == "rotation");
    EXPECT_EQUAL(grid->spec_str(), R"({"grid":"F48","projection":{"south_pole":[30,30],"type":"rotation"}})");

    // first/last points are in the rotated frame
    EXPECT(points_equal(grid->first_point(), PointLonLat{0., 88.572168514007274}));
    EXPECT(points_equal(grid->last_point(), PointLonLat{358.125, -88.572168514007274}));

    const std::vector<PointLonLat> ref{
        {-150.00000000000000, -28.57216851400726}, {-150.05319064425672, -28.57292230625801},
        {-150.10632666115495, -28.57518290821670}, {-150.15935347266884, -28.57894799620847},
        {-150.21221659941676, -28.58421369979395}, {-150.26486170993135, -28.59097460529629},
        {-150.31723466986631, -28.59922376073626}, {-150.36928159111906, -28.60895268217335},
        {-150.42094888084813, -28.62015136144922}, {-150.47218329036292, -28.63280827532991},
    };

    auto points = grid->to_points();
    for (size_t i = 0; i < ref.size(); ++i) {
        EXPECT(points_equal(points[i], ref[i]));
    }
}


CASE("crop") {
    spec::Custom a({{"grid", "f2"}});
    std::unique_ptr<const Grid> grid1(GridFactory::build(a));
    auto n1 = grid1->size();

    EXPECT_EQUAL(n1, 32);

    a.set("south", 0.);
    std::unique_ptr<const Grid> grid2(GridFactory::build(a));
    auto n2 = grid2->size();

    EXPECT_EQUAL(n2, n1 / 2);

    spec::Custom b{{{"grid", "f2"}, {"west", -180}}};
    std::unique_ptr<const Grid> grid3(GridFactory::build(b));
    auto n3 = grid3->size();

    EXPECT_EQUAL(n3, n1);

    auto bbox3 = grid3->boundingBox();

    EXPECT(bbox3.periodic());

    bbox3 = {bbox3.north(), bbox3.west(), bbox3.south(), 0.};

    EXPECT_NOT(bbox3.periodic());

    std::unique_ptr<const Grid> grid4(grid3->make_grid_cropped(bbox3));
    auto n4 = grid4->size();

    EXPECT_EQUAL(n4, 5 * 4);  // Ni * Nj

    b.set("east", -1.);
    std::unique_ptr<const Grid> grid5(GridFactory::build(b));
    auto n5 = grid5->size();

    EXPECT_EQUAL(n5, 4 * 4);  // Ni * Nj

    const std::vector<PointLonLat> ref{
        {-180., 59.444408289},  {-135., 59.444408289},  {-90., 59.444408289},  {-45., 59.444408289},
        {-180., 19.875719147},  {-135., 19.875719147},  {-90., 19.875719147},  {-45., 19.875719147},
        {-180., -19.875719147}, {-135., -19.875719147}, {-90., -19.875719147}, {-45., -19.875719147},
        {-180., -59.444408289}, {-135., -59.444408289}, {-90., -59.444408289}, {-45., -59.444408289},
    };

    auto points5 = grid5->to_points();

    EXPECT(points5.size() == grid5->size());
    ASSERT(points5.size() == ref.size());

    auto it = grid5->begin();
    for (size_t i = 0; i < points5.size(); ++i) {
        EXPECT(points_equal(ref[i], points5[i]));
        EXPECT(points_equal(ref[i], *it));
        ++it;
    }
    EXPECT(it == grid5->end());

    size_t i = 0;
    for (const auto& it : *grid5) {
        EXPECT(points_equal(ref[i++], it));
    }
    EXPECT_EQUAL(i, n5);
}


CASE("equals") {
    std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "f3"}})));
    std::unique_ptr<const Grid> grid2(new RegularGaussian(3));

    EXPECT(*grid1 == *grid2);
}


CASE("scan modes") {
    SECTION("i+j- (default)") {
        // Default scan: i increasing, j decreasing (north to south)
        RegularGaussian grid(1);

        EXPECT(grid.order() == "i+j-");

        const std::vector<PointLonLat> ref{
            {0., 35.264389683},  {90., 35.264389683},  {180., 35.264389683},  {270., 35.264389683},   // north row
            {0., -35.264389683}, {90., -35.264389683}, {180., -35.264389683}, {270., -35.264389683},  // south row
        };

        auto points = grid.to_points();
        ASSERT(points.size() == ref.size());

        for (size_t i = 0; i < points.size(); ++i) {
            EXPECT(points_equal(ref[i], points[i]));
        }
    }

    SECTION("i+j+") {
        // Scan: i increasing, j increasing (south to north)
        RegularGaussian grid(1, {}, order::Scan{"i+j+"});

        EXPECT(grid.order() == "i+j+");

        const std::vector<PointLonLat> ref{
            {0., -35.264389683}, {90., -35.264389683}, {180., -35.264389683}, {270., -35.264389683},  // south row first
            {0., 35.264389683},  {90., 35.264389683},  {180., 35.264389683},  {270., 35.264389683},   // north row last
        };

        auto points = grid.to_points();
        ASSERT(points.size() == ref.size());

        for (size_t i = 0; i < points.size(); ++i) {
            EXPECT(points_equal(ref[i], points[i]));
        }
    }
}


CASE("scanning order: same points and bounding box") {
    const std::vector<std::string> orders{"i+j+", "i-j-", "i-j+", "j-i+", "j+i+", "j-i-", "j+i-"};

    for (const std::string spec : {
             "grid: F8",
             "grid: F8, area: [60, -10, 30, 40]",
             "grid: F16, area: [60, 350, 30, 400]",
             "grid: F8, rotation: [-40, 20]",
         }) {
        const std::unique_ptr<const Grid> canonical(GridFactory::make_from_string("{" + spec + "}"));
        const auto canonical_points = canonical->to_points();

        for (const auto& order : orders) {
            std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{" + spec + ", order: " + order + "}"));

            EXPECT_EQUAL(grid->size(), canonical->size());
            EXPECT(grid->boundingBox() == canonical->boundingBox());

            const auto ren    = grid->reorder(canonical->order());
            const auto points = grid->to_points();
            for (size_t k = 0; k < points.size(); ++k) {
                EXPECT(points_equal(points[k], canonical_points[ren[k]]));
            }
        }
    }
}


CASE("spec round trip: same points") {
    for (const std::string spec : {
             "{grid: F16}",
             "{grid: F16, area: [60, -10, 30, 40]}",
             "{grid: F16, area: [60, 0, 30, 40]}",
             "{grid: F8, area: [89, 350, 1, 400]}",
             "{grid: F8, area: [10, -7, -10, 7]}",
         }) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(spec));
        std::unique_ptr<const Grid> same(GridFactory::make_from_string(grid->spec_str()));

        EXPECT_EQUAL(same->size(), grid->size());
        EXPECT(same->to_latlons() == grid->to_latlons());
    }
}


CASE("crop to a single column") {
    std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{grid: F8, area: [10, -7, -10, 7]}"));

    const auto [lats, lons] = grid->to_latlons();
    EXPECT(lons == std::vector<double>(lats.size(), 0.));
    EXPECT_EQUAL(grid->size(), 2);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
