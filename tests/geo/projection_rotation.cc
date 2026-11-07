// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/figure/UnitSphere.h"
#include "eckit/geo/projection/Composer.h"
#include "eckit/geo/projection/Reverse.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


using P = std::unique_ptr<Projection>;


constexpr double EPS = 1e-6;


CASE("rotation: South Pole out of range") {
    spec::Custom spec({
        {"type", "rotation"},
        {"south_pole_lat", -91.},
        {"south_pole_lon", -361.},
    });

    Point p = PointLonLat{1, 1};
    P projection(Factory<Projection>::instance().get(spec.get_string("type")).create(spec));

    EXPECT(points_equal(p, projection->inv(projection->fwd(p))));
    EXPECT(points_equal(p, projection->fwd(projection->inv(p))));
}


CASE("rotation: round trips") {
    const PointLonLat p(1, 1);
    int delta[] = {-360, -180, -1, 0, 1, 90, 91, 180};

    for (auto a : delta) {
        for (auto b : delta) {
            for (auto c : delta) {
                projection::Rotation rot({0. + static_cast<double>(b), -90. + static_cast<double>(a)},
                                         static_cast<double>(c));
                EXPECT(rot.rotated() == (a % 360 != 0 || (b - c) % 360 != 0));

                EXPECT(points_equal(p, rot.inv(rot.fwd(p)), EPS));
                EXPECT(points_equal(p, rot.fwd(rot.inv(p)), EPS));
            }
        }
    }
}


CASE("rotation: points") {
    const PointLonLat sp(182., -46.7);
    projection::Rotation rot({sp.lon(), sp.lat()}, 0.);

    ASSERT(points_equal(sp.antipode(), PointLonLat{2., 46.7}));


    SECTION("pole point") {
        PointLonLat a(0., 90.);
        auto b = rot.inv(a);
        auto c = rot.fwd(b);

        EXPECT(points_equal(b, sp.antipode(), EPS));
        EXPECT(points_equal(a, c, EPS));
    }


    SECTION("many points") {
        const int Ni = 12;
        const int Nj = 3;

        const PointLonLat ref[]{
            {-178., -46.7},  //
            {-178., -16.7},
            {-178., 13.3},
            {-178., 43.3},
            {-178., 73.3},
            {2., 76.7},
            {2., 46.7},
            {-178., -46.7},
            {-162.623427, -19.469294},
            {-152.023657, 8.654593},
            {-139.574639, 36.436827},
            {-113.108943, 61.431994},
            {-39.882454, 68.008245},
            {2., 46.7},
            {-178., -46.7},
            {-148.834426, -27.310675},
            {-129.263456, -3.837005},
            {-110.791162, 20.054216},
            {-85.879167, 41.365070},
            {-44.424956, 53.295076},
            {2., 46.7},
            {-178., -46.7},
            {-137.907940, -39.070023},
            {-109.601456, -21.339065},
            {-88., 0.},
            {-66.398544, 21.339065},
            {-38.092060, 39.070023},
            {2., 46.7},
            {-178., -46.7},
            {-131.575044, -53.295076},
            {-90.120833, -41.365070},
            {-65.208838, -20.054216},
            {-46.736544, 3.837005},
            {-27.165574, 27.310675},
            {2., 46.7},
            {-178., -46.7},
            {-136.117546, -68.008245},
            {-62.891057, -61.431994},
            {-36.425361, -36.436827},
            {-23.976343, -8.654593},
            {-13.376573, 19.469294},
            {2., 46.7},
            {-178., -46.7},
            {-178., -76.7},
            {2., -73.3},
            {2., -43.3},
            {2., -13.3},
            {2., 16.7},
            {2., 46.7},
            {-178., -46.7},
            {140.117546, -68.008245},
            {66.891057, -61.431994},
            {40.425361, -36.436827},
            {27.976343, -8.654593},
            {17.376573, 19.469294},
            {2., 46.7},
            {-178., -46.7},
            {135.575044, -53.295076},
            {94.120833, -41.365070},
            {69.208838, -20.054216},
            {50.736544, 3.837005},
            {31.165574, 27.310675},
            {2., 46.7},
            {-178., -46.7},
            {141.907940, -39.070023},
            {113.601456, -21.339065},
            {92., 0.},
            {70.398544, 21.339065},
            {42.092060, 39.070023},
            {2., 46.7},
            {-178., -46.7},
            {152.834426, -27.310675},
            {133.263456, -3.837005},
            {114.791162, 20.054216},
            {89.879167, 41.365070},
            {48.424956, 53.295076},
            {2., 46.7},
            {-178., -46.7},
            {166.623427, -19.469294},
            {156.023657, 8.654593},
            {143.574639, 36.436827},
            {117.108943, 61.431994},
            {43.882454, 68.008245},
            {2., 46.7},
        };

        for (int i = 0, k = 0; i < Ni; i++) {
            for (int j = 0; j < 2 * Nj + 1; j++, k++) {
                PointLonLat a(static_cast<double>(i) * 360. / static_cast<double>(Ni),
                              static_cast<double>(j - Nj) * 90. / static_cast<double>(Nj));
                auto b = rot.inv(a);
                auto c = rot.fwd(b);

                EXPECT(points_equal(b, ref[k], EPS));
                EXPECT(points_equal(a, c, EPS));
            }
        }
    }
}


CASE("rotation: reference values (unchanged)") {
    struct test_t {
        PointLonLat south_pole;
        double angle;
        bool reverse;
        struct point_t {
            PointLonLat p;
            PointLonLat fwd;
            PointLonLat inv;
        };
        std::vector<point_t> points;
    };

    auto equal = [](const PointLonLat& a, const PointLonLat& b) {
        constexpr double eps = 1e-13;
        return types::is_approximately_equal(a.lon(), b.lon(), eps) &&
               types::is_approximately_equal(a.lat(), b.lat(), eps);
    };

    for (const auto& test : std::vector<test_t>{
             {{-170, -40},
              0,
              false,
              {
                  {{0, 0}, {164.66018550081202, 48.973538629984482}, {-170, 50}},
                  {{45, 30}, {-98.357608405488492, 59.863378649840755}, {-80.991973918468091, 52.232103403067178}},
                  {{-120, -75}, {162.60862457655958, -48.44580743019327}, {171.15585767188364, -46.056015998070222}},
                  {{200, 89}, {0.22347998625483151, 39.014976050799163}, {10.440450110344683, 39.059467872760983}},
                  {{359.5, -10}, {166.63627038013868, 39.061559302015937}, {-170.64277271017221, 39.99785153021093}},
              }},
             {{10, 30},
              45,
              false,
              {
                  {{0, 0}, {-115.5745998593172, -58.525051110814516}, {-106.56505117707799, 37.761243907035045}},
                  {{45, 30}, {126.04108905836918, -59.810165310934941}, {-170, 30.000000000000011}},
                  {{-120, -75}, {-120.25492619999616, 38.832061508544676}, {6.0147343911104096, 15.453519938899289}},
                  {{200, 89}, {44.801439427321178, -29.015043294064469}, {-170.49253361821448, -30.905391253195102}},
                  {{359.5, -10}, {-119.20755799152265, -48.743333046184155}, {-95.496205493111205, 43.204750530168795}},
              }},
             {{0, -90},
              30,
              false,
              {
                  {{0, 0}, {30, 0}, {-30, 0}},
                  {{45, 30}, {75, 30}, {15, 30}},
                  {{-120, -75}, {-90, -75}, {-150, -75}},
                  {{200, 89}, {230, 89}, {170, 89}},
                  {{359.5, -10}, {389.5, -10}, {329.5, -10}},
              }},
             {{120, -60},
              0,
              true,
              {
                  {{0, 0}, {119.99999999999999, 29.999999999999996}, {-116.56505117707799, 14.477512185929925}},
                  {{45, 30}, {-174.59722268438219, 47.663220464467678}, {-62.035751879637075, 18.719828073965907}},
                  {{-120, -75}, {88.85379626866461, -64.31903787106684}, {159.3590939000303, -50.516843646153468}},
                  {{200, 89}, {-59.334818641605857, 59.058593425220508}, {1.9587647109907336, 59.811781157052422}},
                  {{359.5, -10}, {119.47599789091922, 19.99885680926927}, {-121.48537769414297, 5.7121135915180918}},
              }},
         }) {
        projection::Rotation rotation(test.south_pole, test.angle, test.reverse);
        for (const auto& [p, fwd, inv] : test.points) {
            EXPECT(equal(rotation.fwd(p), fwd));
            EXPECT(equal(rotation.inv(p), inv));
        }
    }
}


CASE("rotation: Cartesian points") {
    for (const auto& [south_pole, angle] : std::vector<std::pair<PointLonLat, double>>{
             {{-170., -40.}, 0.}, {{10., 30.}, 45.}, {{0., -90.}, 30.}, {{0., -90.}, 0.}}) {
        projection::Rotation rotation(south_pole, angle);

        for (const auto& p : std::vector<PointLonLat>{{0., 0.}, {45., 30.}, {-120., -75.}, {200., 89.}}) {
            const auto xyz = figure::UnitSphere::_convertSphericalToCartesian(p);
            EXPECT(points_equal(figure::UnitSphere::_convertCartesianToSpherical(rotation.fwd(xyz)), rotation.fwd(p)));
            EXPECT(points_equal(figure::UnitSphere::_convertCartesianToSpherical(rotation.inv(xyz)), rotation.inv(p)));
        }
    }
}


CASE("rotation: not rotated, by angle, by matrix") {
    const projection::Rotation non_rotated({0., -90.}, 0.);
    const projection::Rotation rotation_angle({0., -90.}, -180.);
    const projection::Rotation rotation_matrix({4., -40.}, 180.);

    EXPECT(not non_rotated.rotated());
    EXPECT(rotation_angle.rotated());
    EXPECT(rotation_matrix.rotated());

    const PointLonLat p[] = {{0., 90.}, {0., 0.}, {270., 25.}, {-180., 45.}};

    struct {
        const projection::Rotation& rotation;
        const PointLonLat a;
        const PointLonLat b;
    } tests[] = {
        {non_rotated, p[0], p[0]},
        {non_rotated, p[1], p[1]},
        {non_rotated, p[2], p[2]},
        {non_rotated, p[3], p[3]},
        {rotation_angle, p[0], {p[0].lon() - 180., p[0].lat()}},
        {rotation_angle, p[1], {p[1].lon() - 180., p[1].lat()}},
        {rotation_angle, p[2], {p[2].lon() - 180., p[2].lat()}},
        {rotation_angle, p[3], {p[3].lon() - 180., p[3].lat()}},
        {rotation_matrix, p[0], {-176., 40.}},
        {rotation_matrix, p[1], {-176., -50.}},
        {rotation_matrix, p[2], {113.657357, 15.762700}},
        {rotation_matrix, p[3], {-176., 85.}},
    };

    for (const auto& test : tests) {
        auto b = test.rotation.inv(test.a);
        EXPECT(points_equal(b, test.b, EPS));

        auto a = test.rotation.fwd(b);
        EXPECT(points_equal(a, test.a, EPS));
    }
}


CASE("rotation: South Pole at -90 (its longitude as the angle)") {
    struct test_t {
        double south_pole_lon;
        double angle;
        double expected_angle;  // the angle less the South Pole longitude, in [-180, 180)
        PointLonLat q;          // on the rotated sphere
        PointLonLat p;          // geographic (inv(q))
    };

    for (const auto& test : std::vector<test_t>{
             {10., 0., -10., {50., 30.}, {60., 30.}},
             {10., 25., 15., {50., 30.}, {35., 30.}},
             {-30., 5., 35., {0., -45.}, {-35., -45.}},
             {170., -40., 150., {-120., 60.}, {90., 60.}},
             {-120., 90., -150., {10., 0.}, {160., 0.}},
             {45., 360., -45., {0., 60.}, {45., 60.}},
         }) {
        const projection::Rotation rotation({test.south_pole_lon, -90.}, test.angle);
        const projection::Rotation equivalent(SOUTH_POLE, test.expected_angle);
        const projection::Rotation close({test.south_pole_lon, -89.999}, test.angle);  // by the rotation matrix

        EXPECT(rotation.rotated());
        EXPECT(points_equal(rotation.south_pole(), SOUTH_POLE));
        EXPECT(types::is_approximately_equal(rotation.angle(), test.expected_angle, PointLonLat::EPS));
        EXPECT(rotation.spec_str() == equivalent.spec_str());

        // from/to the rotated sphere
        EXPECT(points_equal(rotation.inv(test.q), test.p));
        EXPECT(points_equal(rotation.fwd(test.p), test.q));

        // by spec
        std::unique_ptr<const Projection> from_spec(ProjectionFactory::make_from_string(
            "{type: rotation, south_pole_lon: " + std::to_string(test.south_pole_lon) +
            ", south_pole_lat: -90, rotation_angle: " + std::to_string(test.angle) + "}"));
        EXPECT(from_spec->spec_str() == rotation.spec_str());
        EXPECT(points_equal(from_spec->inv(test.q), test.p));

        // round trips, and as the rotation matrix (close to the South Pole)
        for (const auto& q : std::vector<PointLonLat>{{0., 0.}, {50., 30.}, {-120., -45.}, {179., 60.}, test.q}) {
            EXPECT(points_equal(rotation.inv(q), equivalent.inv(q)));
            EXPECT(points_equal(rotation.inv(q), close.inv(q), 1e-2));
            EXPECT(points_equal(rotation.fwd(rotation.inv(q)), q));
            EXPECT(points_equal(rotation.inv(rotation.fwd(q)), q));
        }
    }

    // the South Pole longitude and the angle cancelling out, not a rotation
    for (const auto& [lon, angle] : std::vector<std::pair<double, double>>{{10., 10.}, {-30., 330.}}) {
        const projection::Rotation rotation({lon, -90.}, angle);
        EXPECT_NOT(rotation.rotated());
        EXPECT(points_equal(rotation.inv(PointLonLat{50., 30.}), PointLonLat{50., 30.}));
    }
}


CASE("rotation: reverse (the inverse rotation)") {
    struct test_t {
        PointLonLat south_pole;
        double angle;
        std::string spec;  // reversed, as the inverse rotation
    };

    const std::vector<PointLonLat> points{{0., 0.}, {50., 30.}, {-120., -45.}, {179., 60.}};

    for (const auto& test : std::vector<test_t>{
             {{10., -40.}, 0., R"({"rotation_angle":-170,"south_pole":[-180,-40],"type":"rotation"})"},  // by matrix
             {{10., -40.}, 25., R"({"rotation_angle":-170,"south_pole":[-155,-40],"type":"rotation"})"},
             {{10., -90.}, 25., R"({"rotation_angle":-15,"type":"rotation"})"},  // by angle
             {SOUTH_POLE, 0., "{}"},                                             // not a rotation
         }) {
        const projection::Rotation rotation(test.south_pole, test.angle);
        const projection::Rotation reverse(test.south_pole, test.angle, true);
        const projection::Reverse wrapped(new projection::Rotation(test.south_pole, test.angle));
        const projection::Reverse reversed_reverse(new projection::Rotation(test.south_pole, test.angle, true));

        EXPECT(reverse.spec_str() == test.spec);
        EXPECT_NOT(reverse.reverse());
        EXPECT(reverse.identity() == rotation.identity());

        // as reversed by projection::Reverse, and by the spec
        std::unique_ptr<const Projection> by_spec(ProjectionFactory::build(wrapped.spec()));
        EXPECT(*by_spec == reverse);

        // reversed again, as not reversed
        std::unique_ptr<const Projection> by_spec_again(ProjectionFactory::build(reversed_reverse.spec()));
        EXPECT(*by_spec_again == rotation);

        for (const auto& p : points) {
            EXPECT(points_equal(reverse.fwd(p), rotation.inv(p)));
            EXPECT(points_equal(reverse.inv(p), rotation.fwd(p)));
            EXPECT(points_equal(wrapped.fwd(p), reverse.fwd(p)));
            EXPECT(points_equal(wrapped.inv(p), reverse.inv(p)));
            EXPECT(points_equal(reversed_reverse.fwd(p), rotation.fwd(p)));
            EXPECT(points_equal(reversed_reverse.inv(p), rotation.inv(p)));
        }
    }
}


CASE("rotation: composed") {
    spec::Custom spec({
        {"type", "rotation"},
        {"south_pole_lat", -90.},
        {"south_pole_lon", 0.},
        {"rotation_angle", 45.},
    });

    // compose sequentially
    const auto& builder = Factory<Projection>::instance().get(spec.get_string("type"));
    P composition1(new projection::Composer{
        builder.create(spec),
        builder.create(spec),
        builder.create(spec),
        builder.create(spec),
        builder.create(spec),
        builder.create(spec),
        builder.create(spec),
    });

    dynamic_cast<projection::Composer*>(composition1.get())->emplace_back(builder.create(spec));

    for (auto lat : {0., 10., -10.}) {
        PointLonLat p{0., lat};
        auto q = composition1->fwd(p);

        EXPECT(points_equal(p, q));
        EXPECT(points_equal(p, composition1->inv(q)));

        auto qs = dynamic_cast<projection::Composer*>(composition1.get())->fwd_points(p);
        EXPECT(qs.size() == 8);

        EXPECT(points_equal(qs.front(), PointLonLat{45., lat}));
        EXPECT(points_equal(qs[1], PointLonLat{90., lat}));
        EXPECT(points_equal(qs[2], PointLonLat{135., lat}));
        // ...
        EXPECT(points_equal(qs.back(), p));
    }

    // compose by nesting
    P composition2(builder.create(spec));
    for (size_t i = 1; i < 8; ++i) {
        composition2.reset(projection::Composer::compose_back(composition2.release(), spec));
    }

    for (auto lat : {0., 10., -10.}) {
        PointLonLat p{0., lat};

        auto qs1 = dynamic_cast<projection::Composer*>(composition1.get())->fwd_points(p);
        auto qs2 = dynamic_cast<projection::Composer*>(composition2.get())->fwd_points(p);

        ASSERT(qs1.size() == 8);
        EXPECT(qs2.size() == 2);
        EXPECT(points_equal(qs1[6], qs2[0]));
        EXPECT(points_equal(qs1[7], qs2[1]));
    }
}


CASE("rotation: bounding box (1)") {
    projection::Rotation rotation(spec::Custom({
        {"type", "rotation"},
        {"south_pole_lat", 10.},
        {"south_pole_lon", 20.},
    }));


    struct test_t {
        PointLonLat p;
        PointLonLat q;
    };

    for (const auto& test : std::vector<test_t>{
             {{0., 1.}, {-160., 79.}},
             {{2., 1.}, {-170.369068, 78.821318}},
             {{0., 0.}, {-160., 80.}},
             {{2., 0.}, {-171.370559, 79.803957}},
         }) {
        EXPECT(points_equal(rotation.inv(test.p), test.q, EPS));
    }

    EXPECT_THROWS_AS(auto dummy = area::BoundingBox::make_from_projection(PointXY{0., 0.}, PointXY{2., 1.}, rotation),
                     exception::ProjectionError);

    auto bbox = area::BoundingBox::make_from_projection(PointLonLat{0., 0.}, PointLonLat{2., 1.}, rotation);
    ASSERT(bbox);

    EXPECT(points_equal(PointLonLat{bbox->west(), bbox->north()}, {-171.37056, 80.000001}, EPS));
    EXPECT(points_equal(PointLonLat{bbox->east(), bbox->south()}, {-160., 78.821318}, EPS));
}


CASE("rotation: bounding box (2)") {
    projection::Rotation rotation({22, -40});
    PointLonLat min(-27, 33);
    PointLonLat max(45, 73);

    auto bbox = area::BoundingBox::make_from_projection(min, max, rotation);
    ASSERT(bbox);

    EXPECT(points_equal(PointLonLat{0, bbox->north()}, NORTH_POLE));
    EXPECT(bbox->periodic());
}


using v = std::vector<double>;


bool approx(const v& a, const v& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (!types::is_approximately_equal(a[i], b[i], PointLonLat::EPS)) {
            return false;
        }
    }
    return true;
}


CASE("rotation: vectors") {
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
            EXPECT(points_equal(PointLonLat{back[0][i], back[1][i]}, PointLonLat{lon[i], lat[i]}));
        }
    };

    SECTION("matrix") {
        check(projection::Rotation{PointLonLat{10., -40.}});
    }


    SECTION("angle") {
        check(projection::Rotation{PointLonLat{0., -90.}, 30.});
    }


    SECTION("not rotated") {
        check(projection::Rotation{});
    }


    SECTION("reverse") {
        const projection::Rotation rotation(PointLonLat{10., -40.});
        const projection::Rotation reverse(PointLonLat{10., -40.}, 0., true);
        const projection::Reverse wrapped(new projection::Rotation(PointLonLat{10., -40.}));

        check(reverse);
        check(wrapped);

        const auto a = rotation.inv(lon, lat);
        for (const Projection* projection : std::vector<const Projection*>{&reverse, &wrapped}) {
            const auto b = projection->fwd(lon, lat);
            EXPECT(approx(a[0], b[0]));
            EXPECT(approx(a[1], b[1]));
        }
    }
}


CASE("rotation: spec of two values") {
    for (const std::string spec : {
             "{type: rotation, south_pole: [1]}",
             "{type: rotation, south_pole: [1, 2, 3]}",
             "{type: rotation, rotation: [1]}",
         }) {
        EXPECT_THROWS_AS(std::unique_ptr<const Projection>(ProjectionFactory::make_from_string(spec)),
                         exception::SpecError);
    }
}

}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
