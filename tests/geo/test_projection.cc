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

#include "grit/Projection.h"
#include "grit/param/Map.h"
#include "grit/test.h"


int main(int argc, char* argv[]) {
    using grit::Point;
    using grit::PointLatLon;
    using grit::PointXYZ;
    using Projection = std::unique_ptr<grit::Projection>;


    Point p = PointLatLon{1, 1};


    {
        std::unique_ptr<grit::Projection> projection(grit::ProjectionFactory::build("none", grit::param::Map{}));
        EXPECT(p == projection->inv(p));
        EXPECT(p == projection->fwd(p));
    }


    {
        grit::param::Map param({
            {"projection", "rotation"},
            {"south_pole_lat", -91.},
            {"south_pole_lon", -361.},
        });

        std::unique_ptr<grit::Projection> projection(
            grit::ProjectionFactory::build(param.get_string("projection"), param));

        EXPECT(p == projection->inv(projection->fwd(p)));
        EXPECT(p == projection->fwd(projection->inv(p)));
    }


    {
        Projection s1(grit::ProjectionFactory::build("ll_to_xyz", grit::param::Map({{"R", 1.}})));
        Projection s2(grit::ProjectionFactory::build("ll_to_xyz", grit::param::Map({{"a", 1.}, {"b", 1.}})));
        Projection s3(grit::ProjectionFactory::build("ll_to_xyz", grit::param::Map({{"a", 1.}, {"b", 0.5}})));

        EXPECT(p == s1->inv(s1->fwd(p)));
        EXPECT(p == s2->inv(s2->fwd(p)));
        EXPECT(s1->fwd(p) == s2->fwd(p));

        Point q = PointLatLon{0, 1};

        EXPECT(s1->fwd(q) == s3->fwd(q));
        EXPECT(s2->fwd(q) == s3->fwd(q));

        struct {
            PointLatLon a;
            PointXYZ b;
        } tests[] = {
            {{0, 0}, {1, 0, 0}},
            {{0, 90}, {0, 1, 0}},
            {{0, 180}, {-1, 0, 0}},
            {{0, 270}, {0, -1, 0}},
            {{-90, 0}, {0, 0, -0.5}},
            {{-90, 42}, {0, 0, -0.5}},
            {{90, 0}, {0, 0, 0.5}},
            {{90, 42}, {0, 0, 0.5}},
        };

        for (const auto& test : tests) {
            EXPECT(s3->fwd(test.a) == Point(test.b));
        }
    }
}
