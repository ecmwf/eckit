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
#include "grit/projection/Rotation.h"


int main(int argc, char* argv[]) {
    {
        std::unique_ptr<grit::Projection> unrotated(new grit::projection::Rotation({-90., 0.}, 0.));

        const grit::PointLatLon a(1, 1);
        auto b = unrotated->direct(a);
        auto c = unrotated->inverse(b);
        auto e = unrotated->inverse(a);
        auto f = unrotated->direct(e);

        std::cout << "unrotated: " << a << " -> " << b << " -> " << c << std::endl;
        std::cout << "unrotated: " << a << " -> " << e << " -> " << f << std::endl;
    }

    {
        std::unique_ptr<grit::Projection> angle_only(new grit::projection::Rotation({-90., 0.}, 10.));

        const grit::PointLatLon a(1, 1);
        auto b = angle_only->direct(a);
        auto c = angle_only->inverse(b);
        auto e = angle_only->inverse(a);
        auto f = angle_only->direct(e);

        std::cout << "angle_only: " << a << " -> " << b << " -> " << c << std::endl;
        std::cout << "angle_only: " << a << " -> " << e << " -> " << f << std::endl;
    }

    {
        std::unique_ptr<grit::Projection> angle_vector(new grit::projection::Rotation({-89., 1.}, 10.));

        const grit::PointLatLon a(1, 1);
        auto b = angle_vector->direct(a);
        auto c = angle_vector->inverse(b);
        auto e = angle_vector->inverse(a);
        auto f = angle_vector->direct(e);

        std::cout << "angle_vector: " << a << " -> " << b << " -> " << c << std::endl;
        std::cout << "angle_vector: " << a << " -> " << e << " -> " << f << std::endl;
    }
}
