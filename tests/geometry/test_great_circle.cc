/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <limits>
#include <vector>

#include "eckit/geometry/GreatCircle.h"
#include "eckit/geometry/Point2.h"
#include "eckit/testing/Test.h"


namespace eckit {
namespace test {

using namespace geometry;

struct PointLonLat : Point2 {
    PointLonLat(double x, double y) :
        Point2(x, y) {}
    const double& lon() const { return x_[0]; }
    const double& lat() const { return x_[1]; }
};

// -----------------------------------------------------------------------------
// test great circles

CASE("test great circles intersections") {
    using types::is_approximately_equal;
    using types::is_approximately_greater_or_equal;

    auto is_approximately_equal_longitude = [](double lon1, double lon2,
                                               double epsilon = std::numeric_limits<double>::epsilon()) -> bool {
        while (lon2 < lon1) {
            lon2 += 360;
        }
        while (lon1 >= lon1 + 360) {
            lon2 -= 360;
        }
        return is_approximately_equal(lon1, lon2, epsilon) || is_approximately_equal(lon1, lon2 - 360, epsilon);
    };

    auto is_approximately_pole = [](double lat, double epsilon = std::numeric_limits<double>::epsilon()) -> bool {
        return is_approximately_equal(std::abs(lat), 90., epsilon);
    };

    auto is_approximately_equator = [](double lat, double epsilon = std::numeric_limits<double>::epsilon()) -> bool {
        return is_approximately_equal(lat, 0., epsilon);
    };

    const std::vector<double> latitudes{
        90,
        60,
        45,
        30,
        0,
        -30,
        -45,
        -60,
        -90,
    };

    const std::vector<double> longitudes{
        -181,
        -180,
        -135,
        -90,
        -45,
        0,
        45,
        90,
        135,
        180,
        225,
        270,
        315,
        360,
        361,
    };

    const std::vector<PointLonLat> antipodes{
        {0, 0},
        {180, 0},
        {-180, 0},
        {0, 0},
        {-90, 0},
        {90, 0},
        {90, 0},
        {-90, 0},
        {0, 90},
        {0, -90},
        {0, -90},
        {0, 90},
        {45, 45},
        {225, -45},
    };

    SECTION("example intersection with meridian and parallel") {
        // latitude at Valparaíso-Shanghai mid-point
        const PointLonLat P1(-71.6, -33.);
        const PointLonLat P2(121.8, 31.4);
        GreatCircle gc(P1, P2);

        const PointLonLat mid(-159.18, -6.81);

        auto lats = gc.latitude(mid.lon());
        EXPECT(lats.size() == 1 && is_approximately_equal(lats[0], mid.lat(), 0.01));

        auto lons = gc.longitude(mid.lat());
        EXPECT(lons.size() == 2);
        EXPECT(is_approximately_equal_longitude(lons[0], mid.lon(), 0.01) || is_approximately_equal_longitude(lons[1], mid.lon(), 0.01));
    }

    SECTION("mal-formed great circle") {
        for (size_t i = 0; i < antipodes.size(); i += 2) {
            const PointLonLat& A(antipodes[i]);
            const PointLonLat& B(antipodes[i + 1]);

            EXPECT_THROWS_AS(GreatCircle(A, A), BadValue);
            EXPECT_THROWS_AS(GreatCircle(B, B), BadValue);

            EXPECT_THROWS_AS(GreatCircle(A, B), BadValue);

            if (is_approximately_pole(A.lat())) {
                for (double lon1_gc : longitudes) {
                    for (double lon2_gc : longitudes) {
                        EXPECT_THROWS_AS(GreatCircle({lon1_gc, A.lat()}, {lon2_gc, A.lat()}), BadValue);
                        EXPECT_THROWS_AS(GreatCircle({lon1_gc, B.lat()}, {lon2_gc, B.lat()}), BadValue);
                    }
                }
            }
        }
    }

    SECTION("intersection at quadrants") {
        for (double lat_gc : latitudes) {
            if (!is_approximately_pole(lat_gc) && !is_approximately_equator(lat_gc)) {
                for (double lon_gc : longitudes) {

                    GreatCircle gc({lon_gc, lat_gc}, {lon_gc + 90, 0});
                    EXPECT(!gc.crossesPoles());

                    auto lon_at_equator = gc.longitude(0);
                    EXPECT(lon_at_equator.size() == 2);
                    EXPECT((is_approximately_equal_longitude(lon_gc + 90, lon_at_equator[0]) && is_approximately_equal_longitude(lon_gc - 90, lon_at_equator[1])) || (is_approximately_equal_longitude(lon_gc - 90, lon_at_equator[0]) && is_approximately_equal_longitude(lon_gc + 90, lon_at_equator[1])));

                    auto lon_extrema1 = gc.longitude(lat_gc);
                    EXPECT(lon_extrema1.size() == 1 && is_approximately_equal_longitude(lon_extrema1[0], lon_gc, 0.01));

                    auto lon_extrema2 = gc.longitude(-lat_gc);
                    EXPECT(lon_extrema2.size() == 1 && is_approximately_equal_longitude(lon_extrema2[0], lon_gc + 180, 0.01));
                }
            }
        }
    }

    SECTION("intersection with parallels when crossing the poles") {
        for (double lon : longitudes) {
            for (double lat : latitudes) {

                {
                    GreatCircle gc({lon, -10}, {lon, 10});
                    EXPECT(gc.crossesPoles());

                    auto lons = gc.longitude(lat);
                    size_t N  = is_approximately_pole(lat) ? 1 : 2;
                    EXPECT(lons.size() == N);

                    if (N == 1) {
                        EXPECT(is_approximately_equal_longitude(lons[0], lon));
                    }
                    else {
                        EXPECT(is_approximately_equal_longitude(lons[0] + 180, lons[1]));
                        EXPECT(is_approximately_equal_longitude(lons[0], lon) || is_approximately_equal_longitude(lons[1], lon));
                    }
                }

                if (!is_approximately_pole(lat) && !is_approximately_equator(lat)) {
                    GreatCircle gc({lon, lat}, {lon + 180, lat});
                    EXPECT(gc.crossesPoles());

                    auto lons = gc.longitude(lat);
                    EXPECT(lons.size() == 2);

                    EXPECT(is_approximately_equal_longitude(lons[0] + 180, lons[1]));
                    EXPECT(is_approximately_equal_longitude(lons[0], lon) || is_approximately_equal_longitude(lons[1], lon));
                }
            }
        }
    }

    SECTION("intersection with parallels") {
        for (double lat_gc : latitudes) {
            if (/* avoid mal-forming */ !is_approximately_pole(lat_gc)) {
                for (double lat : latitudes) {

                    GreatCircle gc({-1, lat_gc}, {1, lat_gc});
                    EXPECT(!gc.crossesPoles());

                    auto lons = gc.longitude(lat);
                    size_t N  = is_approximately_equator(lat_gc)                                     ? 0.
                                : is_approximately_greater_or_equal(std::abs(lat_gc), std::abs(lat)) ? 2
                                                                                                     : 0;
                    EXPECT(lons.size() == N);

                    for (auto lon : lons) {
                        auto lats = gc.latitude(lon);
                        EXPECT(lats.size() == 1 && is_approximately_equal(lats[0], lat, 0.01));
                    }
                }
            }
        }
    }

    SECTION("equator great circle intersection with meridian and parallel") {
        for (double lon : longitudes) {

            GreatCircle eq({lon - 1, 0}, {lon + 1, 0});
            EXPECT(!eq.crossesPoles());

            // non-intersection with parallels
            for (double lat : latitudes) {
                EXPECT(eq.longitude(lat).empty());
            }

            // intersect one latitude only, for specific longitudes
            auto lats = eq.latitude(lon);
            EXPECT(lats.size() == 1 && is_approximately_equator(lats[0]));
        }
    }
}

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
