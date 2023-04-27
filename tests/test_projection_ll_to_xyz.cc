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

#include "grit/projection/LatLonToXYZ.h"
#include "grit/test.h"


int main(int argc, char* argv[]) {
    using grit::Point;
    using grit::PointLatLon;
    using grit::projection::LatLonToXYZ;

    const PointLatLon p(1., 723.);


    {
        LatLonToXYZ to_xyz(1.);

        auto q = to_xyz.fwd(p);
        auto r = to_xyz.inv(q);
        std::cout << "p(lat, lon): " << p << " -> p(x,y,z): " << q << " -> p(lat, lon): " << r << std::endl;

        EXPECT(Point(p) == r);
    }


    {
        LatLonToXYZ to_xyz_ab(3., 2.);  // oblate
        LatLonToXYZ to_xyz_ba(2., 3.);  // problate

        for (const auto& lon : {0., 90., 180., 270.}) {
            PointLatLon p{0., lon};
            std::cout << "p(lat, lon): " << p << " -> p_ab(x,y,z): " << to_xyz_ab.fwd(p)
                      << ", p_ba(x,y,z): " << to_xyz_ba.fwd(p) << std::endl;
        }
    }
}
