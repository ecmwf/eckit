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


#include <iostream>

#include "grit/projection/Rotation.h"
#include "grit/test.h"


int main(int argc, char* argv[]) {
    using grit::Point;
    using grit::PointLatLon;
    using grit::projection::Rotation;


    {
        const PointLatLon p(1, 1);
        int delta[] = {-360, -180, -1, 0, 1, 90, 91, 180};

        for (auto a : delta) {
            for (auto b : delta) {
                for (auto c : delta) {
                    Rotation rot(-90. + static_cast<double>(a), 0. + static_cast<double>(b), static_cast<double>(c));
                    EXPECT(rot.rotated() == (a % 360 != 0 || (b - c) % 360 != 0));

                    EXPECT(Point(p) == rot.inv(rot.fwd(p)));
                    EXPECT(Point(p) == rot.fwd(rot.inv(p)));
                }
            }
        }
    }


    {
        const int Ni = 12;
        const int Nj = 3;

        Rotation rot(-46.7, 182., 0.);
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

        for (int i = 0, k = 0; i < Ni; i++) {
            for (int j = 0; j < 2 * Nj + 1; j++, k++) {
                PointLatLon a(static_cast<double>(j - Nj) * 90. / static_cast<double>(Nj),
                              static_cast<double>(i) * 360. / static_cast<double>(Ni));
                auto b = rot.fwd(a);
                EXPECT(Point(b) == ref[k]);
                EXPECT(Point(a) == rot.inv(b));
            }
        }
    }


    {
        const Rotation unrotated(-90., 0., 0.);
        const Rotation angle_only(-90., 0., -180.);
        const Rotation rotation(-40., 4., 180.);

        EXPECT(not unrotated.rotated());
        EXPECT(angle_only.rotated());
        EXPECT(rotation.rotated());

        const PointLatLon p[] = {{90., 0.}, {0., 0.}, {25., 270.}, {45., -180.}};

        struct {
            const Rotation& rotation;
            const PointLatLon a;
            const PointLatLon b;
        } tests[] = {
            {unrotated, p[0], p[0]},
            {unrotated, p[1], p[1]},
            {unrotated, p[2], p[2]},
            {unrotated, p[3], p[3]},
            {angle_only, p[0], {p[0].lat, p[0].lon - 180.}},
            {angle_only, p[1], {p[1].lat, p[1].lon - 180.}},
            {angle_only, p[2], {p[2].lat, p[2].lon - 180.}},
            {angle_only, p[3], {p[3].lat, p[3].lon - 180.}},
            {rotation, p[0], {40., -176.}},
            {rotation, p[1], {-50., -176.}},
            {rotation, p[2], {15.762700, 113.657357}},
            {rotation, p[3], {85., -176.}},
        };

        for (const auto& test : tests) {
            auto b = test.rotation.fwd(test.a);
            EXPECT(Point(b) == test.b);

            auto a = test.rotation.inv(b);
            EXPECT(Point(a) == test.a);
        }
    }
}