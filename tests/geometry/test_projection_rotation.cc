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

#include "eckit/geometry/projection/Rotation.h"
#include "eckit/testing/Test.h"


int main(int argc, char* argv[]) {
    using eckit::geometry::Point;
    using eckit::geometry::PointLonLat;
    using eckit::geometry::points_equal;
    using eckit::geometry::projection::Rotation;

    {
        const PointLonLat p(1, 1);
        int delta[] = {-360, -180, -1, 0, 1, 90, 91, 180};

        for (auto a : delta) {
            for (auto b : delta) {
                for (auto c : delta) {
                    Rotation rot(0. + static_cast<double>(b), -90. + static_cast<double>(a), static_cast<double>(c));
                    EXPECT(rot.rotated() == (a % 360 != 0 || (b - c) % 360 != 0));

                    EXPECT(points_equal(p, rot.inv(rot.fwd(p))));
                    EXPECT(points_equal(p, rot.fwd(rot.inv(p))));
                }
            }
        }
    }

    {
        const int Ni = 12;
        const int Nj = 3;

        Rotation rot(182., -46.7, 0.);
        const PointLonLat ref[]{
            {-178., -46.7},
            {-178., -16.7},
            {-178., 13.3},
            {-178., 43.3},
            {-178., 73.3},
            {2., 76.7},
            {2., 46.7},
            {-178., -46.7},
            {-162.62343, -19.46929},
            {-152.02366, 8.65459},
            {-139.57464, 36.43683},
            {-113.10894, 61.43199},
            {-39.88245, 68.00825},
            {2., 46.7},
            {-178., -46.7},
            {-148.83443, -27.31067},
            {-129.26346, -3.837},
            {-110.79116, 20.05422},
            {-85.87917, 41.36507},
            {-44.42496, 53.29508},
            {2., 46.7},
            {-178., -46.7},
            {-137.90794, -39.07002},
            {-109.60146, -21.33906},
            {-88., 0.},
            {-66.39854, 21.33906},
            {-38.09206, 39.07002},
            {2., 46.7},
            {-178., -46.7},
            {-131.57504, -53.29508},
            {-90.12083, -41.36507},
            {-65.20884, -20.05422},
            {-46.73654, 3.837},
            {-27.16557, 27.31067},
            {2., 46.7},
            {-178., -46.7},
            {-136.11755, -68.00825},
            {-62.89106, -61.43199},
            {-36.42536, -36.43683},
            {-23.97634, -8.65459},
            {-13.37657, 19.46929},
            {2., 46.7},
            {-178., -46.7},
            {-178., -76.7},
            {2., -73.3},
            {2., -43.3},
            {2., -13.3},
            {2., 16.7},
            {2., 46.7},
            {-178., -46.7},
            {140.11755, -68.00825},
            {66.89106, -61.43199},
            {40.42536, -36.43683},
            {27.97634, -8.65459},
            {17.37657, 19.46929},
            {2., 46.7},
            {-178., -46.7},
            {135.57504, -53.29508},
            {94.12083, -41.36507},
            {69.20884, -20.05422},
            {50.73654, 3.837},
            {31.16557, 27.31067},
            {2., 46.7},
            {-178., -46.7},
            {141.90794, -39.07002},
            {113.60146, -21.33906},
            {92., 0.},
            {70.39854, 21.33906},
            {42.09206, 39.07002},
            {2., 46.7},
            {-178., -46.7},
            {152.83443, -27.31067},
            {133.26346, -3.837},
            {114.79116, 20.05422},
            {89.87917, 41.36507},
            {48.42496, 53.29508},
            {2., 46.7},
            {-178., -46.7},
            {166.62343, -19.46929},
            {156.02366, 8.65459},
            {143.57464, 36.43683},
            {117.10894, 61.43199},
            {43.88245, 68.00825},
            {2., 46.7},
        };

        for (int i = 0, k = 0; i < Ni; i++) {
            for (int j = 0; j < 2 * Nj + 1; j++, k++) {
                PointLonLat a(static_cast<double>(i) * 360. / static_cast<double>(Ni),
                              static_cast<double>(j - Nj) * 90. / static_cast<double>(Nj));
                auto b = rot.fwd(a);
                EXPECT(points_equal(b, ref[k]));
                EXPECT(points_equal(a, rot.inv(b)));
            }
        }
    }

    {
        const Rotation non_rotated(0., -90., 0.);
        const Rotation rotation_angle(0., -90., -180.);
        const Rotation rotation_matrix(4., -40., 180.);

        EXPECT(not non_rotated.rotated());
        EXPECT(rotation_angle.rotated());
        EXPECT(rotation_matrix.rotated());

        const PointLonLat p[] = {{0., 90.}, {0., 0.}, {270., 25.}, {-180., 45.}};

        struct {
            const Rotation& rotation;
            const PointLonLat a;
            const PointLonLat b;
        } tests[] = {
            {non_rotated, p[0], p[0]},
            {non_rotated, p[1], p[1]},
            {non_rotated, p[2], p[2]},
            {non_rotated, p[3], p[3]},
            {rotation_angle, p[0], {p[0].lon - 180., p[0].lat}},
            {rotation_angle, p[1], {p[1].lon - 180., p[1].lat}},
            {rotation_angle, p[2], {p[2].lon - 180., p[2].lat}},
            {rotation_angle, p[3], {p[3].lon - 180., p[3].lat}},
            {rotation_matrix, p[0], {-176., 40.}},
            {rotation_matrix, p[1], {-176., -50.}},
            {rotation_matrix, p[2], {113.657357, 15.762700}},
            {rotation_matrix, p[3], {-176., 85.}},
        };

        for (const auto& test : tests) {
            auto b = test.rotation.fwd(test.a);
            EXPECT(points_equal(b, test.b));

            auto a = test.rotation.inv(b);
            EXPECT(points_equal(a, test.a));
        }
    }
}
