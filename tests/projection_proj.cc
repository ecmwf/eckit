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


int main(int argc, char* argv[]) {
    using grit::PointLatLon;
    using grit::PointXY;

    const std::string string1 = "+proj=utm +zone=32 +datum=WGS84";
    const std::string string2 = "EPSG:32632";


    constexpr double eps = 1e-6;

    for (const auto& string : {string1, string2}) {

        grit::projection::PROJ_LatLon_to_XY projection(grit::param::Map({
            {"source", "EPSG:4326"},
            {"target", string},
        }));

        std::cout.precision(16);

        PointLatLon a{55., 12.};
        std::cout << "a: " << a << std::endl;

        auto b = projection.fwd(a);
        std::cout << "b: " << b << std::endl;

        auto c = projection.inv(b);
        std::cout << "c: " << c << std::endl;

        std::cout << b.is_approximately_equal({691875.632137542, 6098907.825129169}, eps) << std::endl;
        std::cout << a.is_approximately_equal(projection.inv(b), eps) << std::endl;
    }
}
