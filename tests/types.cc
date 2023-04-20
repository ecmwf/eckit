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
#include "grit/types.h"


int main(int argc, char* argv[]) {
    using grit::PointLatLon;

    PointLatLon p(90., 1);

    std::cout << "p: " << p << std::endl;
    std::cout << "p: " << PointLatLon::make(p.lat, p.lon) << std::endl;
    std::cout << "p: " << (p == PointLatLon(90., 50.)) << std::endl;

    PointLatLon q(-90., 1.);
    std::cout << "q: " << q << std::endl;
    std::cout << "~q: " << q.antipode() << std::endl;
    std::cout << "~~q: " << q.antipode().antipode() << std::endl;

    auto r(PointLatLon::make(-91., -10.));
    ASSERT(r == r.antipode().antipode());

    ASSERT(PointLatLon(-30.000000000000018, -59.99999999999996).is_approximately_equal({-30, 300}, 1e-5));
    ASSERT(PointLatLon(-46.7, -178.00000000000003).is_approximately_equal({-46.7, -178.}, 1e-5));
}
