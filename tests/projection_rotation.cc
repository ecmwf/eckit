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


#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "grit/Projection.h"
#include "grit/exception.h"
#include "grit/projection/Rotation.h"
#include "grit/util.h"


using grit::PointLatLon;
using grit::projection::Rotation;

static const PointLatLon SP{-90., 0.};
static const PointLatLon NP{90., 180.};


bool equivalent(const PointLatLon& a, const PointLatLon& b) {
    auto f  = [=](double lon) { return 10. + std::cos(lon * grit::util::degrees_to_radians); };
    auto eq = [](double a, double b) { return grit::util::is_approximately_equal(a, b, 1e-5); };

    auto ok = (eq(a.lat, 90.) && eq(b.lat, 90.)) || (eq(a.lat, -90.) && eq(b.lat, -90.)) ||
              eq(a.lat, b.lat) && eq(f(a.lon), f(b.lon));

    std::cout << a << " <=> " << b << " ? " << ok << std::endl;
    return ok;
}


void eqv(const Rotation& rot, const PointLatLon& a, const PointLatLon& b) {
    auto bn  = rot.direct(a);
    auto bok = equivalent(bn, b);
    //    std::cout << a << " -> " << bn << " == " << b << " ? " << bok << std::endl;

    auto an  = rot.inverse(b);
    auto aok = equivalent(an, a);
    //    std::cout << b << " <- " << an << " == " << a << " ? " << aok << std::endl;

    ASSERT(aok && bok);
}


int main(int argc, char* argv[]) {
    {
        std::unique_ptr<grit::Projection> unrotated(new Rotation(-90., 0., 0.));

        const PointLatLon a(1, 1);
        auto b = unrotated->direct(a);
        auto c = unrotated->inverse(b);
        auto e = unrotated->inverse(a);
        auto f = unrotated->direct(e);

        std::cout << "unrotated: " << a << " -> " << b << " -> " << c << std::endl;
        std::cout << "unrotated: " << a << " -> " << e << " -> " << f << std::endl;
    }

    {
        std::unique_ptr<grit::Projection> angle_only(new Rotation(-90., 0., 10.));

        const PointLatLon a(1, 1);
        auto b = angle_only->direct(a);
        auto c = angle_only->inverse(b);
        auto e = angle_only->inverse(a);
        auto f = angle_only->direct(e);

        std::cout << "angle_only: " << a << " -> " << b << " -> " << c << std::endl;
        std::cout << "angle_only: " << a << " -> " << e << " -> " << f << std::endl;
    }

    {
        std::unique_ptr<grit::Projection> angle_vector(new Rotation(-89., 1., 10.));

        const PointLatLon a(1, 1);
        auto b = angle_vector->direct(a);
        auto c = angle_vector->inverse(b);
        auto e = angle_vector->inverse(a);
        auto f = angle_vector->direct(e);

        std::cout << "angle_vector: " << a << " -> " << b << " -> " << c << std::endl;
        std::cout << "angle_vector: " << a << " -> " << e << " -> " << f << std::endl;
    }

    {
        const int nx = 12;
        const int ny = 6;


        Rotation rot(-133.3, 2.,
                     180.);  // NP (lat, lon) = (46.7, 2., 180.) (SP cannot be expressed canonically for the same point)

        const PointLatLon ref[]{
            {-46.7, -178.},
            {-16.7, -178.},
            {13.3, -178.},
            {43.3, -178.},
            {73.3, -178.},
            {76.7, 2.},
            {46.7, 2.},
            {-46.7, -178.},
            {-19.46929, -162.62343},
            {8.65459, -152.02366},
            {36.43683, -139.57464},
            {61.43199, -113.10894},
            {68.00825, -39.88245},
            {46.7, 2.},
            {-46.7, -178.},
            {-27.31067, -148.83443},
            {-3.837, -129.26346},
            {20.05422, -110.79116},
            {41.36507, -85.87917},
            {53.29508, -44.42496},
            {46.7, 2.},
            {-46.7, -178.},
            {-39.07002, -137.90794},
            {-21.33906, -109.60146},
            {0., -88.},
            {21.33906, -66.39854},
            {39.07002, -38.09206},
            {46.7, 2.},
            {-46.7, -178.},
            {-53.29508, -131.57504},
            {-41.36507, -90.12083},
            {-20.05422, -65.20884},
            {3.837, -46.73654},
            {27.31067, -27.16557},
            {46.7, 2.},
            {-46.7, -178.},
            {-68.00825, -136.11755},
            {-61.43199, -62.89106},
            {-36.43683, -36.42536},
            {-8.65459, -23.97634},
            {19.46929, -13.37657},
            {46.7, 2.},
            {-46.7, -178.},
            {-76.7, -178.},
            {-73.3, 2.},
            {-43.3, 2.},
            {-13.3, 2.},
            {16.7, 2.},
            {46.7, 2.},
            {-46.7, -178.},
            {-68.00825, 140.11755},
            {-61.43199, 66.89106},
            {-36.43683, 40.42536},
            {-8.65459, 27.97634},
            {19.46929, 17.37657},
            {46.7, 2.},
            {-46.7, -178.},
            {-53.29508, 135.57504},
            {-41.36507, 94.12083},
            {-20.05422, 69.20884},
            {3.837, 50.73654},
            {27.31067, 31.16557},
            {46.7, 2.},
            {-46.7, -178.},
            {-39.07002, 141.90794},
            {-21.33906, 113.60146},
            {0., 92.},
            {21.33906, 70.39854},
            {39.07002, 42.09206},
            {46.7, 2.},
            {-46.7, -178.},
            {-27.31067, 152.83443},
            {-3.837, 133.26346},
            {20.05422, 114.79116},
            {41.36507, 89.87917},
            {53.29508, 48.42496},
            {46.7, 2.},
            {-46.7, -178.},
            {-19.46929, 166.62343},
            {8.65459, 156.02366},
            {36.43683, 143.57464},
            {61.43199, 117.10894},
            {68.00825, 43.88245},
            {46.7, 2.},
        };

        for (int i = 0, jglo = 0; i < nx; i++) {
            for (int j = 0; j < ny + 1; j++, jglo++) {
                double lon = static_cast<double>(i) * 360. / static_cast<double>(nx);
                double lat = static_cast<double>(j - ny / 2) * 90. / static_cast<double>(ny / 2);
                PointLatLon p0(lat, lon);
                auto p1 = rot.direct(p0);
                auto p2 = rot.inverse(p1);
                ASSERT(equivalent(p2, p0));
                ASSERT(equivalent(p1, ref[jglo]));
            }
        }
    }


    {
        for (const auto& p : {SP, NP, PointLatLon{-89.9, 0.}, PointLatLon{89.9, 0.}}) {
            Rotation rotation(p.lat, p.lon, 0.);
            ASSERT(rotation.rotated() == (p != SP));
        }
    }


    {
        using P  = PointLatLon;
        using PQ = std::pair<PointLatLon, PointLatLon>;

        Rotation unrotated(SP.lat, SP.lon, 0.);
        ASSERT(not unrotated.rotated());

        for (const auto& p : {P{90., 0.}, P{0., 0.}, P{25., 270.}, P{45., -180.}}) {
            eqv(unrotated, p, p);
        }

        Rotation angle_only(SP.lat, SP.lon, -180.);
        ASSERT(angle_only.rotated());

        for (const auto& p : {P{90., 0.}, P{0., 0.}, P{25., 270.}, P{45., -180.}}) {
            eqv(angle_only, p, {p.lat, p.lon - 180.});
        }

        Rotation rotation(-40., 4., 0.);
        ASSERT(rotation.rotated());

        eqv(rotation, {90., 0.}, {40., -176.});
        //        eqv(rotation, {0., 0.}, {-50., -176.});
        //        eqv(rotation, {45., -180.}, {85., -176.});

#if 0
    Config config;
    config.set("north_pole", std::vector<double>{-176, 40});
    Rotation rotation(config);
    Log::info() << rotation << std::endl;

    EXPECT(rotation.rotated());

    PointLonLat p, r;

    p = {0., 90.};
    r = {-176., 40.};
    EXPECT_EQUIVALENT(rotation.rotate(p), r);
    EXPECT_EQUIVALENT(rotation.unrotate(r), p);

    p = {0., 0.};
    r = {-176., -50.};
    EXPECT_EQUIVALENT(rotation.rotate(p), r);
    EXPECT_EQUIVALENT(rotation.unrotate(r), p);

    p = {-180., 45.};
    r = {-176., 85.};
    EXPECT_EQUIVALENT(rotation.rotate(p), r);
    EXPECT_EQUIVALENT(rotation.unrotate(r), p);
#endif
    }
}
