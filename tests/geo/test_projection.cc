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

#include "eckit/geo/Projection.h"
#include "eckit/geo/param/Map.h"
#include "eckit/testing/Test.h"


int main(int argc, char* argv[]) {
    using eckit::geo::Point;
    using eckit::geo::Point3;
    using eckit::geo::PointLonLat;
    using Projection = std::unique_ptr<eckit::geo::Projection>;

    Point p = PointLonLat{1, 1};

    {
        std::unique_ptr<eckit::geo::Projection> projection(
            eckit::geo::ProjectionFactory::build("none", eckit::geo::param::Map{}));
        EXPECT(p == projection->inv(p));
        EXPECT(p == projection->fwd(p));
    }

    {
        eckit::geo::param::Map param({
            {"projection", "rotation"},
            {"south_pole_lat", -91.},
            {"south_pole_lon", -361.},
        });

        std::unique_ptr<eckit::geo::Projection> projection(
            eckit::geo::ProjectionFactory::build(param.get_string("projection"), param));

        EXPECT(p == projection->inv(projection->fwd(p)));
        EXPECT(p == projection->fwd(projection->inv(p)));
    }

    {
        Projection s1(
            eckit::geo::ProjectionFactory::build("ll_to_xyz", eckit::geo::param::Map({{"R", 1.}})));
        Projection s2(
            eckit::geo::ProjectionFactory::build("ll_to_xyz",
                                                 eckit::geo::param::Map({{"a", 1.}, {"b", 1.}})));
        Projection s3(
            eckit::geo::ProjectionFactory::build("ll_to_xyz",
                                                 eckit::geo::param::Map({{"a", 1.}, {"b", 0.5}})));

        EXPECT(p == s1->inv(s1->fwd(p)));
        EXPECT(p == s2->inv(s2->fwd(p)));
        EXPECT(s1->fwd(p) == s2->fwd(p));

        Point q = PointLonLat{1, 0};

        EXPECT(s1->fwd(q) == s3->fwd(q));
        EXPECT(s2->fwd(q) == s3->fwd(q));

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
            EXPECT(s3->fwd(test.a) == Point(test.b));
        }
    }
}
