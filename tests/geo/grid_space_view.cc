// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("space_view: as GRIB") {
    // sectors of the (GRIB) full disc: Nr=6.6107, dx=3622, dy=3622 (or 3610), Xp=Yp=1856; points as the eccodes
    // iterator (and PROJ geos), to microdegrees
    constexpr double microdegree = 1e-6;

    struct test_t {
        std::string spec;
        std::vector<PointLonLat> first_second_last;
        std::string regular_xy;
    };

    const std::vector<test_t> tests{
        {R"({"dx":3622,"dy":3622,"nr":6.6107,"nx":16,"ny":16,"type":"space_view","xo":1500,"xp":1856,"yo":1700,"yp":1856})",
         {{350.299524805, 4.223693723}, {350.327193534, 4.223622147}, {350.719698129, 3.815601110}},
         R"({"false_xy":[1067074.48,-467594.435],"projection":{"h":35747054.5503,"reverse":true,"type":"space_view"},)"
         R"("x":[0,44961.0033558134,2997.40022372089],"y":[0,-44961.0033558134,-2997.40022372089]})"},
        {R"({"dx":3622,"dy":3610,"figure":"wgs84","lon_0":-75,"nr":6.6107,"nx":16,"ny":12,"type":"space_view",)"
         R"("xo":2400,"xp":1856,"yo":1200,"yp":1856})",
         {{300.905565350, 18.425011761}, {300.936080607, 18.425558651}, {301.327135202, 18.106822780}},
         R"({"false_xy":[-1632353.683,-1968348.046],)"
         R"("projection":{"figure":"wgs84","h":35785813.2659,"lon_0":-75,"reverse":true,"type":"space_view"},)"
         R"("x":[0,45009.7522881123,3000.65015254082],"y":[0,-33005.8361442917,-3000.53055857197]})"},
    };
    for (const auto& test : tests) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string(test.spec));

        EXPECT_EQUAL(grid->type(), "space_view");
        EXPECT_EQUAL(grid->spec_str(), test.spec);

        const auto points = grid->to_points();
        EXPECT(points_equal(points.front(), test.first_second_last[0], microdegree));
        EXPECT(points_equal(points[1], test.first_second_last[1], microdegree));
        EXPECT(points_equal(points.back(), test.first_second_last[2], microdegree));

        // the same points as the regular_xy of the space_view projection (to millimetres on the projection plane)
        std::unique_ptr<const Grid> xy(GridFactory::make_from_string(test.regular_xy));
        const auto xy_points = xy->to_points();
        EXPECT_EQUAL(points.size(), xy_points.size());
        for (size_t i = 0; i < points.size(); ++i) {
            EXPECT(points_equal(points[i], xy_points[i], microdegree));
        }

        // round trip
        std::unique_ptr<const Grid> same(GridFactory::make_from_string(grid->spec_str()));
        EXPECT(*same == *grid);
    }
}


CASE("space_view: order") {
    const std::string common =
        R"("dx":3622,"dy":3622,"nr":6.6107,"nx":4,"ny":3,"type":"space_view","xo":1850,"xp":1856,"yo":1850,"yp":1856)";

    std::unique_ptr<const Grid> ij(GridFactory::make_from_string("{" + common + "}"));
    EXPECT_EQUAL(ij->order(), "i+j-");

    const std::vector<std::string> orders{"i-j+", "i+j+", "i-j-", "j-i+", "i+-j-"};
    for (const std::string& order : orders) {
        std::unique_ptr<const Grid> grid(GridFactory::make_from_string("{" + common + R"(,"order":")" + order + "\"}"));
        EXPECT_EQUAL(grid->order(), order);
        EXPECT(grid->spec_str().find(order) != std::string::npos);
        EXPECT_EQUAL(grid->size(), ij->size());
    }

    // i- j+ (as MSG): the points mirrored about the sub-satellite point
    std::unique_ptr<const Grid> mirrored(GridFactory::make_from_string("{" + common + R"(,"order":"i-j+"})"));
    const auto a  = ij->to_points();
    const auto b  = mirrored->to_points();
    const auto& p = std::get<PointLonLat>(a.front());
    const auto& q = std::get<PointLonLat>(b.front());
    EXPECT(types::is_approximately_equal(PointLonLat::normalise_angle_to_minimum(p.lon(), -180.),
                                         -PointLonLat::normalise_angle_to_minimum(q.lon(), -180.), 1e-9));
    EXPECT(types::is_approximately_equal(p.lat(), -q.lat(), 1e-9));
}


CASE("space_view: errors") {
    const std::vector<std::string> specs{
        R"({"type":"space_view","dx":3622,"dy":3622,"nr":6.6107,"ny":3})",
        R"({"type":"space_view","dx":0,"dy":3622,"nr":6.6107,"nx":4,"ny":3})",
        R"({"type":"space_view","dx":3622,"dy":3622,"nx":4,"ny":3})",
        R"({"type":"space_view","dx":3622,"dy":3622,"nr":1,"nx":4,"ny":3})",
        R"({"type":"space_view","dx":3622,"dy":3622,"nr":0.5,"nx":4,"ny":3})",
    };
    for (const auto& spec : specs) {
        EXPECT_THROWS_AS(GridFactory::make_from_string(spec), exception::SpecError);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
