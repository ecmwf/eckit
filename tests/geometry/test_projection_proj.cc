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

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geometry/projection/PROJ.h"
#include "eckit/testing/Test.h"


int main(int argc, char* argv[]) {
    using eckit::geometry::Point;
    using eckit::geometry::Point2;
    using eckit::geometry::Point3;
    using eckit::geometry::PointLonLat;
    using eckit::geometry::points_equal;
    using eckit::geometry::projection::PROJ;

    std::cout.precision(14);

    PointLonLat a{12., 55.};

    struct {
        const Point b;
        const std::string target;
    } tests[] = {
        {Point2{691875.632137542, 6098907.825129169}, "+proj=utm +zone=32 +datum=WGS84"},
        {Point2{691875.632137542, 6098907.825129169}, "EPSG:32632"},
        {a, "EPSG:4326"},
        {a, "EPSG:4979"},
        {Point3{3586469.6567764, 762327.65877826, 5201383.5232023}, "EPSG:4978"},
        {Point3{3574529.7050235, 759789.74368715, 5219005.2599833},
         "+proj=cart +R=6371229."},
        {Point3{3574399.5431832, 759762.07693392, 5218815.216709},
         "+proj=cart +ellps=sphere"},
        {a, "+proj=latlon +ellps=sphere"},
    };

    for (const auto& test : tests) {
        PROJ projection(eckit::MappedConfiguration({{"source", "EPSG:4326"}, {"target", test.target}}));

        std::cout << "ellipsoid: '" << PROJ::ellipsoid(projection.target())
                  << std::endl;

        auto b = projection.fwd(a);
        auto c = projection.inv(b);

        std::cout << "-> a:" << a << " -> fwd(a):" << b << " -> inv(fwd(a)):" << c << std::endl;

        EXPECT(points_equal(b, test.b));
        EXPECT(points_equal(c, a));

        PROJ reverse(eckit::MappedConfiguration({{"source", test.target}, {"target", "EPSG:4326"}}));

        auto d = reverse.fwd(test.b);
        auto e = reverse.inv(d);

        std::cout << "-> b:" << test.b << " -> fwd(b):" << d << " -> inv(fwd(b)):" << e << std::endl;

        EXPECT(points_equal(d, a));
        EXPECT(points_equal(e, test.b));
    }
}