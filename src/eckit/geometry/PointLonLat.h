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
#include <cmath>
#include <ostream>

#include "eckit/exception/Exceptions.h"


namespace eckit::geometry {


class PointLonLat final : protected std::array<double, 2> {
private:
    // -- Types

    using P = std::array<double, 2>;

    // -- Class methods

    static double normal(double a, double minimum) {
        while (a < minimum) {
            a += 360.;
        }

        while (a >= minimum + 360.) {
            a -= 360.;
        }

        return a;
    };

public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    PointLonLat(double lat, double lon) :
        P{lat, lon} { ASSERT_MSG(-90. <= lat && lat <= 90., "PointLonLat: invalid latitude"); }
    PointLonLat(const PointLonLat& other) :
        P(other) {}
    PointLonLat(PointLonLat&& other) :
        P(other) {}

    // -- Destructor

    ~PointLonLat() = default;

    // -- Convertors
    // None

    // -- Operators

    PointLonLat& operator=(const PointLonLat& other) {
        P::operator=(other);
        return *this;
    }

    PointLonLat& operator=(PointLonLat&& other) {
        P::operator=(other);
        return *this;
    }

    bool is_approximately_equal(const PointLonLat& other, double eps) const {
        const auto dlon = normal(other.lon, lon) - lon;
        return std::abs(lat - other.lat) < eps && (std::abs(lat - 90.) < eps || std::abs(lat + 90.) < eps || dlon < eps || dlon - 360. < eps);
    };

    // -- Members

    double& lat = P::operator[](0);
    double& lon = P::operator[](1);

    // -- Methods

    static PointLonLat make(double lat, double lon, double lon_minimum = 0) {
        lat = normal(lat, -90.);

        if (lat > 90.) {
            lat = 180. - lat;
            lon += 180.;
        }

        return {lat, lat == -90. || lat == 90. ? 0. : normal(lon, lon_minimum)};
    }

    PointLonLat antipode() const { return make(lat + 180., lon); }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointLonLat& p) {
        return out << '{' << p.lat << ", " << p.lon << '}';
    }
};

bool points_equal(const PointLonLat&, const PointLonLat&);

}  // namespace eckit::geometry
