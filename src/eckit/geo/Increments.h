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


#pragma once

#include <array>
#include <ostream>


namespace eckit {
class Configuration;
}


namespace eckit::geo {


class Increments : protected std::array<double, 2> {
private:
    // -- Types

    using P = std::array<double, 2>;

public:
    // -- Constructors

    Increments(double west_east, double south_north) :
        P{west_east, south_north} {};

    Increments(const Configuration&);

    // -- Members

    double& west_east   = P::operator[](0);
    double& south_north = P::operator[](1);

    // -- Friends

    friend std::ostream& operator<<(std::ostream&out, const Increments&p) {
        return out << '{' << p.west_east << ", " << p.south_north << '}';
    }
};


}  // namespace eckit::geo
