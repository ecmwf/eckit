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

#include "grit/test.h"
#include "grit/types.h"
#include "grit/types/MatrixXYZ.h"


int main(int argc, char* argv[]) {
    using grit::Point;
    using grit::PointLatLon;

    PointLatLon p(90., 1);

    std::cout << "p: " << p << std::endl;
    std::cout << "p: " << PointLatLon::make(p.lat, p.lon) << std::endl;
    std::cout << "p: " << (Point(p) == PointLatLon(90., 50.)) << std::endl;

    PointLatLon q(-90., 1.);
    std::cout << "q: " << q << std::endl;
    std::cout << "~q: " << q.antipode() << std::endl;
    std::cout << "~~q: " << q.antipode().antipode() << std::endl;

    auto r(PointLatLon::make(-91., -10.));
    EXPECT(Point(r) == r.antipode().antipode());

    Point a1 = PointLatLon{-30, 300};
    Point a2 = PointLatLon{-30.000000000000018, -59.99999999999996};
    EXPECT(a1 == a2);

    Point b1 = PointLatLon{-46.7, -178.};
    Point b2 = PointLatLon{-46.7, -178.00000000000003};
    EXPECT(b1 == b2);


    {
        using MatrixXYZ = grit::types::MatrixXYZ<double>;
        MatrixXYZ M{1, 2, 3,  //
                    4, 5, 6,  //
                    7, 8, 1};
        const auto W = M.inverse();
        std::cout << "M M^-1=" << (M * W) << std::endl;
        std::cout << "M^-1 M=" << (W * M) << std::endl;
    }
}
