// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "eckit/geo/Projection.h"
#include "eckit/geo/figure/UnitSphere.h"
#include "eckit/geo/projection/Composer.h"
#include "eckit/geo/projection/EquidistantCylindrical.h"
#include "eckit/geo/projection/LonLatToXYZ.h"  // to test Reverse
#include "eckit/geo/projection/None.h"
#include "eckit/geo/projection/Reverse.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


CASE("projection: none") {
    Point p = PointLonLat{1, 1};
    std::unique_ptr<Projection> projection(ProjectionFactoryType::instance().get("none").create(spec::Custom{}));

    EXPECT(points_equal(p, projection->inv(p)));
    EXPECT(points_equal(p, projection->fwd(p)));
}


CASE("projection: reverse") {
    projection::LonLatToXYZ ab(new figure::UnitSphere);
    projection::Reverse<projection::LonLatToXYZ> ba(new figure::UnitSphere);

    PointLonLat p = NORTH_POLE;
    PointXYZ q{0., 0., 1.};

    ASSERT(points_equal(q, ab.fwd(p)));
    ASSERT(points_equal(p, ab.inv(q)));

    // ensure fwd(PointXYZ) -> PointLonLat, inv(PointLonLat) -> PointXYZ
    EXPECT(points_equal(p, ba.fwd(q)));
    EXPECT(points_equal(q, ba.inv(p)));

    ASSERT(ab.spec().get_string("type") == "ll-to-xyz");
    EXPECT(ba.spec().get_string("type") == "reverse-ll-to-xyz");
}


using v = std::vector<double>;


bool approx(const v& a, const v& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (!types::is_approximately_equal(a[i], b[i], 1e-9)) {
            return false;
        }
    }
    return true;
}


/// Project one point, given and returning one value per coordinate
v single(const std::vector<v>& vs) {
    v q;
    for (const auto& c : vs) {
        ASSERT(c.size() == 1);
        q.push_back(c.front());
    }
    return q;
}


v fwd(const Projection& projection, const v& p) {
    return single(projection.fwd(v{p.at(0)}, v{p.at(1)}, p.size() > 2 ? v{p[2]} : v{}));
}


v inv(const Projection& projection, const v& q) {
    return single(projection.inv(v{q.at(0)}, v{q.at(1)}, q.size() > 2 ? v{q[2]} : v{}));
}


bool all_nan(const v& a) {
    return std::all_of(a.begin(), a.end(), [](double x) { return std::isnan(x); });
}


/// lonlat to xy (identity), failing for negative longitudes
struct Failing final : Projection {
    Point fwd(const Point& p) const override {
        const auto& q = std::get<PointLonLat>(p);
        if (q.lon() < 0.) {
            throw BadValue("Failing: negative longitude", Here());
        }
        return PointXY{q.lon(), q.lat()};
    }

    Point inv(const Point& q) const override {
        const auto& p = std::get<PointXY>(q);
        return PointLonLat{p.X(), p.Y()};
    }

    const std::string& type() const override {
        static const std::string type{"failing"};
        return type;
    }
};


/// lonlat to xy, but not honouring its target point type
struct Dishonest final : Projection {
    Point fwd(const Point& p) const override { return p; }
    Point inv(const Point& q) const override { return q; }

    const std::string& type() const override {
        static const std::string type{"dishonest"};
        return type;
    }
};


/// a projection of the empty point (not a point type)
struct Empty final : Projection {
    Empty() : Projection(nullptr, Point{}, PointXY{}) {}
    Point fwd(const Point& p) const override { return p; }
    Point inv(const Point& q) const override { return q; }

    const std::string& type() const override {
        static const std::string type{"empty"};
        return type;
    }
};


CASE("projection: point types") {
    using names = std::vector<std::string>;

    auto check = [](const Projection& projection, const names& source, const names& target) {
        EXPECT(projection.source_point_coordinates() == source);
        EXPECT(projection.target_point_coordinates() == target);
    };

    const auto& LL  = point_coordinates<PointLonLat>();
    const auto& XY  = point_coordinates<PointXY>();
    const auto& XYZ = point_coordinates<PointXYZ>();

    SECTION("empty point") {
        EXPECT_THROWS_AS(Empty{}, BadValue);
    }


    SECTION("by class") {
        check(projection::None{}, LL, LL);
        check(projection::Rotation{PointLonLat{10., -40.}}, LL, LL);
        check(projection::EquidistantCylindrical{}, LL, XY);
        check(projection::LonLatToXYZ{new figure::UnitSphere}, LL, XYZ);
        check(projection::Reverse<projection::LonLatToXYZ>{new figure::UnitSphere}, XYZ, LL);
    }


    SECTION("composer") {
        projection::Composer composer;
        check(composer, LL, XY);  // empty, the default

        composer.emplace_back(new projection::Rotation{PointLonLat{10., -40.}});
        check(composer, LL, LL);

        composer.emplace_back(new projection::LonLatToXYZ{new figure::UnitSphere});
        check(composer, LL, XYZ);

        projection::Composer to_lonlat{new projection::Reverse<projection::LonLatToXYZ>{new figure::UnitSphere},
                                       new projection::Rotation{PointLonLat{10., -40.}}};
        check(to_lonlat, XYZ, LL);
    }
}


CASE("projection: coordinates") {
    SECTION("lonlat to xyz") {
        projection::LonLatToXYZ to_xyz(new figure::UnitSphere);
        const Projection& projection = to_xyz;

        EXPECT(approx(fwd(projection, v{0., 0.}), v{1., 0., 0.}));
        EXPECT(approx(fwd(projection, v{90., 0.}), v{0., 1., 0.}));
        EXPECT(approx(inv(projection, v{0., 0., 1.}), v{0., 90.}));

        EXPECT_THROWS_AS(fwd(projection, v{0., 0., 0.}), BadValue);  // a lonlat point has 2 coordinates
        EXPECT_THROWS_AS(inv(projection, v{1., 0.}), BadValue);      // a xyz point has 3 coordinates
    }


    SECTION("reverse") {
        projection::Reverse<projection::LonLatToXYZ> to_lonlat(new figure::UnitSphere);
        const Projection& projection = to_lonlat;

        EXPECT(approx(fwd(projection, v{0., 1., 0.}), v{90., 0.}));
        EXPECT(approx(inv(projection, v{90., 0.}), v{0., 1., 0.}));
    }


    SECTION("by spec") {
        std::unique_ptr<const Projection> rotation(
            ProjectionFactory::build(spec::Custom{{"type", "rotation"}, {"south_pole", v{10., -40.}}}));
        EXPECT(approx(fwd(*rotation, v{0., 0.}), v{10., 50.}));
        EXPECT(approx(inv(*rotation, fwd(*rotation, v{10., 20.})), v{10., 20.}));

        std::unique_ptr<const Projection> eqc(ProjectionFactory::build(spec::Custom{{"type", "eqc"}, {"lat_ts", 60.}}));
        EXPECT(approx(fwd(*eqc, v{10., 20.}), v{5., 20.}));
        EXPECT(approx(inv(*eqc, v{5., 20.}), v{10., 20.}));
    }


    SECTION("failure is NaN") {
        const Failing projection;
        const Projection& failing = projection;
        EXPECT(approx(fwd(failing, v{10., 20.}), v{10., 20.}));
        EXPECT(all_nan(fwd(failing, v{-10., 20.})));
    }
}


CASE("projection: vectors") {
    projection::LonLatToXYZ to_xyz(new figure::UnitSphere);
    const Projection& projection = to_xyz;

    // three points: (lon, lat) = (0, 0), (90, 0), (0, 90)
    const v lon{0., 90., 0.};
    const v lat{0., 0., 90.};

    SECTION("fwd/inv") {
        const auto xyz = projection.fwd(lon, lat);
        EXPECT(xyz.size() == 3);

        EXPECT(approx(xyz[0], v{1., 0., 0.}));
        EXPECT(approx(xyz[1], v{0., 1., 0.}));
        EXPECT(approx(xyz[2], v{0., 0., 1.}));

        const auto ll = projection.inv(xyz[0], xyz[1], xyz[2]);
        EXPECT(ll.size() == 2);

        EXPECT(approx(ll[1], lat));
        EXPECT(approx(ll[0], v{0., 90., ll[0][2]}));  // longitude at the pole is arbitrary
    }


    SECTION("no points") {
        const auto xyz = projection.fwd(v{}, v{});
        EXPECT(xyz.size() == 3);
        EXPECT(xyz[0].empty() && xyz[1].empty() && xyz[2].empty());
    }


    SECTION("number and sizes of vectors") {
        EXPECT_THROWS_AS(projection.fwd(lon, lat, lat), BadValue);   // lonlat points have 2 coordinates
        EXPECT_THROWS_AS(projection.inv(lon, lat), BadValue);        // xyz points have 3 coordinates
        EXPECT_THROWS_AS(projection.fwd(lon, v{0., 0.}), BadValue);  // different sizes
    }


    SECTION("failure is NaN, for the failing points only") {
        const Failing projection_failing;
        const Projection& failing = projection_failing;

        const auto xy = failing.fwd(v{10., -10., 30.}, v{20., 20., 40.});

        EXPECT(approx(v{xy[0][0], xy[0][2]}, v{10., 30.}));
        EXPECT(approx(v{xy[1][0], xy[1][2]}, v{20., 40.}));
        EXPECT(std::isnan(xy[0][1]) && std::isnan(xy[1][1]));
    }


    SECTION("projection not honouring its point types") {
        const Dishonest projection_dishonest;  // declares lonlat to xy, returns lonlat
        const Projection& dishonest = projection_dishonest;
        EXPECT_THROWS_AS(fwd(dishonest, v{1., 2.}), std::bad_variant_access);
    }
}


CASE("projection: vectors, rotation") {
    const v lon{0., 10., -30., 170., 45.};
    const v lat{0., 20., 45., -60., 89.};

    // vectors (specialised) match points (one at a time)
    auto check = [&](const Projection& projection) {
        const auto fwd_ll = projection.fwd(lon, lat);
        const auto inv_ll = projection.inv(lon, lat);

        for (size_t i = 0; i < lon.size(); ++i) {
            const auto q = std::get<PointLonLat>(projection.fwd(PointLonLat{lon[i], lat[i]}));
            EXPECT(approx(v{fwd_ll[0][i], fwd_ll[1][i]}, v{q.lon(), q.lat()}));

            const auto p = std::get<PointLonLat>(projection.inv(PointLonLat{lon[i], lat[i]}));
            EXPECT(approx(v{inv_ll[0][i], inv_ll[1][i]}, v{p.lon(), p.lat()}));
        }

        // and round trip
        const auto back = projection.inv(fwd_ll[0], fwd_ll[1]);
        for (size_t i = 0; i < lon.size(); ++i) {
            EXPECT(points_equal(PointLonLat{back[0][i], back[1][i]}, PointLonLat{lon[i], lat[i]}, 1e-9));
        }
    };

    SECTION("matrix") {
        check(projection::Rotation{PointLonLat{10., -40.}});
    }


    SECTION("angle") {
        check(projection::Rotation{PointLonLat{0., -90.}, 30.});
    }


    SECTION("none") {
        check(projection::Rotation{});
    }


    SECTION("reverse") {
        projection::Reverse<projection::Rotation> reverse(PointLonLat{10., -40.});
        projection::Rotation rotation(PointLonLat{10., -40.});

        const auto a = static_cast<const Projection&>(reverse).fwd(lon, lat);
        const auto b = static_cast<const Projection&>(rotation).inv(lon, lat);

        EXPECT(approx(a[0], b[0]));
        EXPECT(approx(a[1], b[1]));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
