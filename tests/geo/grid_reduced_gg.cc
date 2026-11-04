// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cmath>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/grid/reduced/ReducedGaussian.h"
#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


using grid::reduced::ReducedGaussian;


CASE("gridspec") {
    // different ways to instantiate the same grid (O2)
    for (const auto& spec : {
             spec::Custom({{"grid", "o2"}}),
             spec::Custom({{"N", 2}}),
             spec::Custom({{"pl", pl_type{20, 24, 24, 20}}}),
         }) {
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec));
        auto n1 = grid1->size();

        EXPECT(n1 == 88);
        EXPECT(grid1->spec_str() == R"({"grid":"O2"})");

        spec::Custom nhemisphere(spec.container());
        nhemisphere.set("south", 0);

        std::unique_ptr<const Grid> grid2(GridFactory::build(nhemisphere));
        auto n2 = grid2->size();

        EXPECT(n2 == n1 / 2);
        EXPECT(grid2->spec_str() == R"({"area":[90,0,19.8757191474409,360],"grid":"O2"})");

        spec::Custom shemisphere(spec.container());
        shemisphere.set("north", 0);

        std::unique_ptr<const Grid> grid3(GridFactory::build(shemisphere));
        auto n3 = grid3->size();

        EXPECT(n3 == n2);
        EXPECT(grid3->spec_str() == R"({"area":[-19.8757191474409,0,-90,360],"grid":"O2"})");
    }
}


CASE("name") {
    struct test_t {
        const std::string spec;
        const std::string name;
    };

    for (const auto& test : std::vector<test_t>{
             {"{grid: o2}", "O2"},
             {"{N: 2}", "O2"},
             {"{pl: [20, 24, 24, 20]}", "O2"},
             {"{grid: n32}", "N32"},
         }) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.spec));

        EXPECT(grid->name() == test.name);
        EXPECT(grid->arrangement().empty());
    }

    EXPECT(ReducedGaussian(3).octahedral());
    EXPECT(ReducedGaussian(3).name() == "O3");
}


CASE("sizes") {
    struct test_t {
        explicit test_t(size_t N) : N(N), size(4 * N * (N + 9)) {}
        size_t N;
        size_t size;
    } tests[]{test_t{2}, test_t{3}, test_t{64}};

    for (const auto& test : tests) {
        std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "o" + std::to_string(test.N)}})));
        std::unique_ptr<const Grid> grid2(GridFactory::build(spec::Custom({{"type", "reduced_gg"}, {"N", test.N}})));
        ReducedGaussian grid3(test.N);

        EXPECT(grid1->size() == test.size);
        EXPECT(grid2->size() == test.size);
        EXPECT(grid3.size() == test.size);
    }
}


CASE("points") {
    ReducedGaussian grid(1);

    const std::vector<PointLonLat> ref{
        {0., 35.264389683},    {18., 35.264389683},   {36., 35.264389683},   {54., 35.264389683},
        {72., 35.264389683},   {90., 35.264389683},   {108., 35.264389683},  {126., 35.264389683},
        {144., 35.264389683},  {162., 35.264389683},  {180., 35.264389683},  {198., 35.264389683},
        {216., 35.264389683},  {234., 35.264389683},  {252., 35.264389683},  {270., 35.264389683},
        {288., 35.264389683},  {306., 35.264389683},  {324., 35.264389683},  {342., 35.264389683},
        {0., -35.264389683},   {18., -35.264389683},  {36., -35.264389683},  {54., -35.264389683},
        {72., -35.264389683},  {90., -35.264389683},  {108., -35.264389683}, {126., -35.264389683},
        {144., -35.264389683}, {162., -35.264389683}, {180., -35.264389683}, {198., -35.264389683},
        {216., -35.264389683}, {234., -35.264389683}, {252., -35.264389683}, {270., -35.264389683},
        {288., -35.264389683}, {306., -35.264389683}, {324., -35.264389683}, {342., -35.264389683},
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


CASE("crop") {
    spec::Custom a({{"grid", "o2"}});
    std::unique_ptr<const Grid> grid1(GridFactory::build(a));
    auto n1 = grid1->size();

    EXPECT(n1 == 88);

    a.set("south", 0.);
    std::unique_ptr<const Grid> grid2(GridFactory::build(a));
    auto n2 = grid2->size();

    EXPECT(n2 == n1 / 2);

    spec::Custom b{{{"grid", "o2"}, {"west", -180}}};
    std::unique_ptr<const Grid> grid3(GridFactory::build(b));
    auto n3 = grid3->size();

    EXPECT(n3 == n1);

    EXPECT(grid3->boundingBox().periodic());
    EXPECT(grid3->boundingBox().spec_str() == R"({"area":[90,-180,-90,180]})");

    // (exclude Greenwhich meridian)
    std::unique_ptr<const Grid> grid4(grid3->make_grid_cropped(area::BoundingBox(90., -180., 0., -1.e-6)));

    auto n4 = grid4->size();
    EXPECT(n4 == n3 / 4);

    const std::vector<PointLonLat> ref{
        {-180., 59.444408289}, {-162., 59.444408289}, {-144., 59.444408289}, {-126., 59.444408289},
        {-108., 59.444408289}, {-90., 59.444408289},  {-72., 59.444408289},  {-54., 59.444408289},
        {-36., 59.444408289},  {-18., 59.444408289},  {-180., 19.875719147}, {-165., 19.875719147},
        {-150., 19.875719147}, {-135., 19.875719147}, {-120., 19.875719147}, {-105., 19.875719147},
        {-90., 19.875719147},  {-75., 19.875719147},  {-60., 19.875719147},  {-45., 19.875719147},
        {-30., 19.875719147},  {-15., 19.875719147},
    };

    auto points4 = grid4->to_points();

    EXPECT(points4.size() == n4);
    ASSERT(points4.size() == ref.size());

    auto it = grid4->begin();
    for (size_t i = 0; i < points4.size(); ++i) {
        EXPECT(points_equal(ref[i], points4[i]));
        EXPECT(points_equal(ref[i], *it));
        ++it;
    }
    EXPECT(it == grid4->end());

    size_t i = 0;
    for (const auto& it : *grid4) {
        EXPECT(points_equal(ref[i++], it));
    }
    EXPECT(i == n4);
}


CASE("equals") {
    std::unique_ptr<const Grid> grid1(GridFactory::build(spec::Custom({{"grid", "o3"}})));
    std::unique_ptr<const Grid> grid2(GridFactory::make_from_string("N: 3"));
    std::unique_ptr<const Grid> grid3(new ReducedGaussian(3));
    std::unique_ptr<const Grid> grid4(new ReducedGaussian(pl_type{20, 24, 28, 28, 24, 20}));

    EXPECT(*grid1 == *grid2);
    EXPECT(*grid2 == *grid3);
    EXPECT(*grid3 == *grid4);
    EXPECT(*grid4 == *grid1);
}


CASE("cropped octahedral (zero-padded pl)") {
    constexpr size_t N = 16;

    const auto& pl_global = util::reduced_octahedral_pl(N);
    const auto lats       = range::GaussianLatitude(N, false).values();

    // contiguous band of rows, the others zeroed (as in a GRIB sub-area)
    constexpr size_t first = 6;
    constexpr size_t last  = 25;

    pl_type pl(pl_global.size(), 0);
    for (size_t j = first; j <= last; ++j) {
        pl[j] = pl_global[j];
    }

    const area::BoundingBox bbox(lats[first], -25., lats[last], 55.);

    SECTION("zero-padded octahedral pl") {
        ReducedGaussian grid(pl, bbox);

        EXPECT(grid.octahedral());
        EXPECT(grid.name() == "O16");

        const auto& spec = grid.spec();
        EXPECT(spec.get_string("grid") == "O16");
        EXPECT(!spec.has("pl"));
        EXPECT(spec.has("area"));

        // the spec builds the same grid
        std::unique_ptr<const Grid> rebuilt(GridFactory::make_from_string(grid.spec_str()));
        EXPECT(rebuilt->size() == grid.size());
        EXPECT(rebuilt->size() < ReducedGaussian(N).size());
        EXPECT(rebuilt->name() == "O16");

        auto a = grid.to_points();
        auto b = rebuilt->to_points();
        ASSERT(a.size() == b.size());
        for (size_t i = 0; i < a.size(); ++i) {
            EXPECT(points_equal(a[i], b[i]));
        }
    }

    SECTION("zero-padded classical pl") {
        ASSERT(util::reduced_classical_pl_known(N));
        const auto& pl_classical = util::reduced_classical_pl(N);

        pl_type pl_c(pl_classical.size(), 0);
        for (size_t j = first; j <= last; ++j) {
            pl_c[j] = pl_classical[j];
        }

        ReducedGaussian grid(pl_c, bbox);

        EXPECT(!grid.octahedral());
        EXPECT(grid.name() == "N16");

        const auto& spec = grid.spec();
        EXPECT(spec.get_string("grid") == "N16");
        EXPECT(!spec.has("pl"));

        std::unique_ptr<const Grid> rebuilt(GridFactory::make_from_string(grid.spec_str()));
        EXPECT(rebuilt->size() == grid.size());
    }

    SECTION("pl matching neither octahedral nor classical") {
        auto pl_other = pl;
        pl_other[first + 1] += 4;

        ReducedGaussian grid(pl_other, bbox);

        EXPECT(!grid.octahedral());

        const auto& spec = grid.spec();
        EXPECT(spec.has("pl"));

        std::unique_ptr<const Grid> rebuilt(GridFactory::make_from_string(grid.spec_str()));
        EXPECT(rebuilt->size() == grid.size());
    }
}


CASE("zero rows inside the band") {
    // a pl with zero rows between non-zero rows is not a cropped global pl: the area cannot describe the gap, so the
    // pl must be kept in the spec (rebuilding from "O<N>" and the area would bring the absent rows back)
    // compare points row by row (the grid iterator does not support empty rows between non-empty ones)
    auto same_points = [](const Grid& a, const Grid& b) {
        const auto* ra = dynamic_cast<const grid::Reduced*>(&a);
        const auto* rb = dynamic_cast<const grid::Reduced*>(&b);
        ASSERT(ra != nullptr && rb != nullptr);
        ASSERT(ra->latitudes().size() == rb->latitudes().size());

        for (size_t j = 0; j < ra->latitudes().size(); ++j) {
            EXPECT(std::abs(ra->latitudes()[j] - rb->latitudes()[j]) < 1e-9);

            const auto& lonsa = ra->longitudes(j);
            const auto& lonsb = rb->longitudes(j);
            ASSERT(lonsa.size() == lonsb.size());
            for (size_t i = 0; i < lonsa.size(); ++i) {
                EXPECT(std::abs(lonsa[i] - lonsb[i]) < 1e-9);
            }
        }
    };

    SECTION("global, N=2") {
        const ReducedGaussian global(2);
        const ReducedGaussian grid(pl_type{20, 0, 24, 20});

        EXPECT(global.size() == 88);
        EXPECT(grid.size() == 64);

        EXPECT(!grid.octahedral());
        EXPECT(grid.name() == "N2");
        EXPECT(grid.spec().has("pl"));

        // (no classical pl is known for N=2, so the name "N2" cannot be resolved: rebuild from the pl itself)
        std::unique_ptr<const Grid> rebuilt(GridFactory::build(spec::Custom({{"pl", grid.spec().get_long_vector("pl")}})));
        EXPECT(rebuilt->size() == grid.size());
        same_points(grid, *rebuilt);
        EXPECT(*rebuilt == grid);

        // not the global O2 grid
        EXPECT(!(grid == global));
        EXPECT(grid.uid() != global.uid());
        EXPECT(rebuilt->uid() == grid.uid());
    }

    SECTION("cropped, N=16") {
        constexpr size_t N     = 16;
        constexpr size_t first = 6;
        constexpr size_t last  = 25;
        constexpr size_t gap   = 10;

        const auto& pl_global = util::reduced_octahedral_pl(N);
        const auto lats       = range::GaussianLatitude(N, false).values();

        const area::BoundingBox bbox(lats[first], -25., lats[last], 55.);

        pl_type pl(pl_global.size(), 0);
        pl_type pl_nogap(pl_global.size(), 0);
        for (size_t j = first; j <= last; ++j) {
            pl[j]       = j == gap ? 0 : pl_global[j];
            pl_nogap[j] = pl_global[j];
        }

        const ReducedGaussian grid(pl, bbox);

        // the same area without the gap (cropped octahedral)
        std::unique_ptr<const Grid> cropped(new ReducedGaussian(pl_nogap, bbox));

        EXPECT(cropped->name() == "O16");
        EXPECT(!grid.octahedral());
        EXPECT(grid.name() == "N16");
        EXPECT(grid.spec().has("pl"));
        EXPECT(grid.size() < cropped->size());

        std::unique_ptr<const Grid> rebuilt(GridFactory::make_from_string(grid.spec_str()));
        EXPECT(rebuilt->size() == grid.size());
        same_points(grid, *rebuilt);

        EXPECT(!(grid == *cropped));
        EXPECT(grid.uid() != cropped->uid());
    }
}


CASE("single retained row") {
    constexpr size_t N = 16;

    // the first row of the classical and the octahedral pl coincide (20 points), a single row cannot tell them apart
    ASSERT(util::reduced_classical_pl_known(N));
    ASSERT(util::reduced_classical_pl(N)[0] == util::reduced_octahedral_pl(N)[0]);
    ASSERT(util::reduced_classical_pl(N)[1] != util::reduced_octahedral_pl(N)[1]);

    const auto& pl_global = util::reduced_octahedral_pl(N);
    const auto lats       = range::GaussianLatitude(N, false).values();

    for (size_t j : {size_t{0}, size_t{5}}) {
        pl_type pl(pl_global.size(), 0);
        pl[j] = pl_global[j];

        const ReducedGaussian grid(pl, area::BoundingBox(lats[j], -25., lats[j], 55.));

        // octahedral takes precedence, and no pl is needed
        EXPECT(grid.octahedral());
        EXPECT(grid.name() == "O16");

        const auto& spec = grid.spec();
        EXPECT(spec.get_string("grid") == "O16");
        EXPECT(!spec.has("pl"));
        EXPECT(spec.has("area"));

        std::unique_ptr<const Grid> rebuilt(GridFactory::make_from_string(grid.spec_str()));
        EXPECT(rebuilt->name() == "O16");
        EXPECT(rebuilt->size() == grid.size());
        EXPECT(grid.size() > 0);
        EXPECT(grid.size() < ReducedGaussian(N).size());

        auto a = grid.to_points();
        auto b = rebuilt->to_points();
        ASSERT(a.size() == b.size());
        for (size_t i = 0; i < a.size(); ++i) {
            EXPECT(points_equal(a[i], b[i]));
        }
    }
}


CASE("spec pl") {
    // global grids: octahedral and classical have no pl, anything else keeps it
    {
        ReducedGaussian grid(16);
        EXPECT(grid.octahedral());
        EXPECT(grid.name() == "O16");
        EXPECT(grid.spec_str() == R"({"grid":"O16"})");
    }

    {
        ASSERT(util::reduced_classical_pl_known(16));
        ReducedGaussian grid(util::reduced_classical_pl(16));
        EXPECT(!grid.octahedral());
        EXPECT(grid.name() == "N16");
        EXPECT(grid.spec_str() == R"({"grid":"N16"})");
    }

    {
        // known N, but a pl which is neither octahedral nor classical
        auto pl = util::reduced_classical_pl(16);
        pl[3] += 2;

        ReducedGaussian grid(pl);
        EXPECT(!grid.octahedral());

        const auto& spec = grid.spec();
        EXPECT(spec.has("pl"));

        std::unique_ptr<const Grid> rebuilt(GridFactory::make_from_string(grid.spec_str()));
        EXPECT(rebuilt->size() == grid.size());
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
