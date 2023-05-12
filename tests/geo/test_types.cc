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

#include "eckit/geo/Point.h"
#include "eckit/maths/Matrix3.h"
#include "eckit/testing/Test.h"


int main(int argc, char* argv[]) {
    using namespace eckit::geo;

    PointLonLat p(1, 90.);

    std::cout << "p: " << p << std::endl;
    std::cout << "p: " << PointLonLat::make(p.lon, p.lat) << std::endl;
    std::cout << "p: " << points_equal(p, PointLonLat(50., 90.)) << std::endl;

    PointLonLat q(1., -90.);
    std::cout << "q: " << q << std::endl;
    std::cout << "~q: " << q.antipode() << std::endl;
    std::cout << "~~q: " << q.antipode().antipode() << std::endl;

    auto r(PointLonLat::make(-10., -91.));
    EXPECT(points_equal(r, r.antipode().antipode()));

    Point a1 = PointLonLat{300, -30};
    Point a2 = PointLonLat{-59.99999999999996, -30.000000000000018};
    EXPECT(points_equal(a1, a2));

    Point b1 = PointLonLat{-178., -46.7};
    Point b2 = PointLonLat{-178.00000000000003, -46.7};
    EXPECT(points_equal(b1, b2));

    {
        using Matrix3 = eckit::maths::Matrix3<double>;
        Matrix3 M{1,
                  2,
                  3,  //
                  4,
                  5,
                  6,  //
                  7,
                  8,
                  1};
        const auto W = M.inverse();
        std::cout << "M M^-1=" << (M * W) << std::endl;
        std::cout << "M^-1 M=" << (W * M) << std::endl;
    }
}
