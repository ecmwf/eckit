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

#include "eckit/geo/param/Map.h"
#include "eckit/geo/projection/PROJ.h"
#include "eckit/testing/Test.h"


int main(int argc, char* argv[]) {
    std::cout.precision(14);

    eckit::geo::PointLonLat a{12., 55.};

    struct {
        const eckit::geo::Point b;
        const std::string target;
    } tests[] = {
        {eckit::geo::Point2{691875.632137542, 6098907.825129169}, "+proj=utm +zone=32 +datum=WGS84"},
        {eckit::geo::Point2{691875.632137542, 6098907.825129169}, "EPSG:32632"},
        {a, "EPSG:4326"},
        {a, "EPSG:4979"},
        {eckit::geo::Point3{3586469.6567764, 762327.65877826, 5201383.5232023}, "EPSG:4978"},
        {eckit::geo::Point3{3574529.7050235, 759789.74368715, 5219005.2599833},
         "+proj=cart +R=6371229."},
        {eckit::geo::Point3{3574399.5431832, 759762.07693392, 5218815.216709},
         "+proj=cart +ellps=sphere"},
        {a, "+proj=latlon +ellps=sphere"},
    };

    for (const auto& test : tests) {
        eckit::geo::projection::PROJ projection(
            eckit::geo::param::Map({{"source", "EPSG:4326"}, {"target", test.target}}));

        std::cout << "ellipsoid: '" << eckit::geo::projection::PROJ::ellipsoid(projection.target())
                  << std::endl;

        auto b = projection.fwd(a);
        auto c = projection.inv(b);

        std::cout << "-> a:" << a << " -> fwd(a):" << b << " -> inv(fwd(a)):" << c << std::endl;

        EXPECT(b == test.b);
        EXPECT(c == a);

        eckit::geo::projection::PROJ reverse(
            eckit::geo::param::Map({{"source", test.target}, {"target", "EPSG:4326"}}));

        auto d = reverse.fwd(test.b);
        auto e = reverse.inv(d);

        std::cout << "-> b:" << test.b << " -> fwd(b):" << d << " -> inv(fwd(b)):" << e << std::endl;

        EXPECT(d == a);
        EXPECT(e == test.b);
    }
}
