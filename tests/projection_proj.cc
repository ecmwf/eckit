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

#include "grit/exception.h"
#include "grit/param/Map.h"
#include "grit/projection/PROJ.h"
#include "grit/types.h"


bool operator==(const grit::Point& p, const grit::Point& q) {
    ASSERT(p.index() == q.index());

    constexpr double eps = 1e-6;

    return std::holds_alternative<grit::PointLatLon>(p)
               ? std::get<grit::PointLatLon>(p).is_approximately_equal(std::get<grit::PointLatLon>(q), eps)
           : std::holds_alternative<grit::PointXY>(p)
               ? std::get<grit::PointXY>(p).is_approximately_equal(std::get<grit::PointXY>(q), eps)
           : std::holds_alternative<grit::PointXYZ>(p)
               ? std::get<grit::PointXYZ>(p).is_approximately_equal(std::get<grit::PointXYZ>(q), eps)
               : NOTIMP;
}


int main(int argc, char* argv[]) {
    std::cout.precision(14);

    grit::PointLatLon a{55., 12.};

    struct {
        const grit::Point b;
        const std::string target;
    } tests[] = {
        {grit::PointXY{691875.632137542, 6098907.825129169}, "+proj=utm +zone=32 +datum=WGS84"},
        {grit::PointXY{691875.632137542, 6098907.825129169}, "EPSG:32632"},
        {a, "EPSG:4326"},
        {a, "EPSG:4979"},
        {grit::PointXYZ{3586469.6567764, 762327.65877826, 5201383.5232023}, "EPSG:4978"},
        {grit::PointXYZ{3574529.7050235, 759789.74368715, 5219005.2599833}, "+proj=cart +R=6371229."},
        {grit::PointXYZ{3574399.5431832, 759762.07693392, 5218815.216709}, "+proj=cart +ellps=sphere"},
        {a, "+proj=latlon +ellps=sphere"},
    };

    for (const auto& test : tests) {
        grit::projection::PROJ projection(grit::param::Map({{"source", "EPSG:4326"}, {"target", test.target}}));

        std::cout << "ellipsoid: '" << grit::projection::PROJ::ellipsoid(projection.target()) << std::endl;

        auto b = projection.fwd(a);
        auto c = projection.inv(b);

        std::cout << "-> a:" << a << " -> fwd(a):" << b << " -> inv(fwd(a)):" << c << std::endl;

        ASSERT(b == test.b);
        ASSERT(c == a);

        grit::projection::PROJ reverse(grit::param::Map({{"source", test.target}, {"target", "EPSG:4326"}}));

        auto d = reverse.fwd(test.b);
        auto e = reverse.inv(d);

        std::cout << "-> b:" << test.b << " -> fwd(b):" << d << " -> inv(fwd(b)):" << e << std::endl;

        ASSERT(d == a);
        ASSERT(e == test.b);
    }
}
