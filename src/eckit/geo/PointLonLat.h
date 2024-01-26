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


namespace eckit::geo {


class PointLonLat final : protected std::array<double, 2> {
private:
    // -- Types

    using P = std::array<double, 2>;

public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    PointLonLat(double lon, double lat) :
        P{lon, lat} {
        if (!(-90. <= lat && lat <= 90.)) {
            throw BadValue("PointLonLat: invalid latitude");
        }
    }

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

    // -- Members

    double& lon = P::operator[](0);
    double& lat = P::operator[](1);

    // -- Methods

    static double normalise_angle_to_minimum(double, double minimum);

    static double normalise_angle_to_maximum(double, double maximum);

    static PointLonLat make(double lon, double lat, double lon_minimum = 0) {
        lat = normalise_angle_to_minimum(lat, -90.);

        if (lat > 90.) {
            lat = 180. - lat;
            lon += 180.;
        }

        return {lat == -90. || lat == 90. ? 0. : normalise_angle_to_minimum(lon, lon_minimum), lat};
    }

    PointLonLat antipode() const { return make(lon, lat + 180.); }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods

    static PointLonLat componentsMin(const PointLonLat& p, const PointLonLat& q) {
        return {std::min(p.lon, q.lon), std::min(p.lat, q.lat)};
    }

    static PointLonLat componentsMax(const PointLonLat& p, const PointLonLat& q) {
        return {std::max(p.lon, q.lon), std::max(p.lat, q.lat)};
    }

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointLonLat& p) {
        return out << '{' << p.lon << ", " << p.lat << '}';
    }

    friend PointLonLat operator-(const PointLonLat& p, const PointLonLat& q) { return {p.lon - q.lon, p.lat - q.lat}; }

    friend PointLonLat operator+(const PointLonLat& p, const PointLonLat& q) { return {p.lon + q.lon, p.lat + q.lat}; }

    friend PointLonLat operator*(const PointLonLat& p, double d) { return {p.lon * d, p.lat * d}; }

    friend bool operator<(const PointLonLat& p, const PointLonLat& q) {
        return static_cast<const P&>(p) < static_cast<const P&>(q);
    }
};

bool points_equal(const PointLonLat&, const PointLonLat&);

}  // namespace eckit::geo
