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
#include <memory>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/grid/Projection.h"
#include "eckit/testing/Test.h"


int main(int argc, char* argv[]) {
    using eckit::MappedConfiguration;

    using eckit::grid::Point;
    using eckit::grid::Point3;
    using eckit::grid::PointLonLat;
    using eckit::grid::points_equal;

    using Projection        = std::unique_ptr<eckit::grid::Projection>;
    using ProjectionFactory = eckit::grid::ProjectionFactory;

    Point p = PointLonLat{1, 1};

    {
        Projection projection(ProjectionFactory::build("none", MappedConfiguration{}));
        EXPECT(points_equal(p, projection->inv(p)));
        EXPECT(points_equal(p, projection->fwd(p)));
    }

    {
        MappedConfiguration param({
            {"projection", "rotation"},
            {"south_pole_lat", -91.},
            {"south_pole_lon", -361.},
        });

        Projection projection(ProjectionFactory::build(param.getString("projection"), param));

        EXPECT(points_equal(p, projection->inv(projection->fwd(p))));
        EXPECT(points_equal(p, projection->fwd(projection->inv(p))));
    }

    {
        Projection s1(ProjectionFactory::build("ll_to_xyz", MappedConfiguration({{"R", 1.}})));
        Projection s2(ProjectionFactory::build("ll_to_xyz", MappedConfiguration({{"a", 1.}, {"b", 1.}})));
        Projection s3(ProjectionFactory::build("ll_to_xyz", MappedConfiguration({{"a", 1.}, {"b", 0.5}})));

        EXPECT(points_equal(p, s1->inv(s1->fwd(p))));
        EXPECT(points_equal(p, s2->inv(s2->fwd(p))));
        EXPECT(points_equal(s1->fwd(p), s2->fwd(p)));

        Point q = PointLonLat{1, 0};

        EXPECT(points_equal(s1->fwd(q), s3->fwd(q)));
        EXPECT(points_equal(s2->fwd(q), s3->fwd(q)));

        struct {
            PointLonLat a;
            Point3 b;
        } tests[] = {
            {{0, 0}, {1, 0, 0}},
            {{90, 0}, {0, 1, 0}},
            {{180, 0}, {-1, 0, 0}},
            {{270, 0}, {0, -1, 0}},
            {{0, -90}, {0, 0, -0.5}},
            {{42, -90}, {0, 0, -0.5}},
            {{0, 90}, {0, 0, 0.5}},
            {{42, 90}, {0, 0, 0.5}},
        };

        for (const auto& test : tests) {
            EXPECT(points_equal(s3->fwd(test.a), test.b));
        }
    }
}
