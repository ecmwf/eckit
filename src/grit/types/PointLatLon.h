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

#include "grit/exception.h"


namespace grit::types {


template <typename T>
class PointLatLon final : protected std::array<T, 2> {
private:
    // -- Types

    using P = std::array<T, 2>;

public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    PointLatLon(T lat, T lon) : P{lat, lon} { ASSERT_MSG(-90. <= lat && lat <= 90., "PointLatLon: invalid latitude"); }
    PointLatLon(const PointLatLon<T>& other) : P(other) {}
    PointLatLon(PointLatLon<T>&& other) : P(other) {}

    // -- Destructor

    ~PointLatLon() = default;

    // -- Convertors
    // None

    // -- Operators

    PointLatLon& operator=(const PointLatLon<T>& other) {
        P::operator=(other);
        return *this;
    }

    PointLatLon& operator=(PointLatLon<T>&& other) {
        P::operator=(other);
        return *this;
    }

    bool operator==(const PointLatLon<T>& other) const {
        return P(make_normalised(lat, lon)) == P(make_normalised(other.lat, other.lon));
    }

    bool operator!=(const PointLatLon<T>& other) const { return !operator==(other); }

    // -- Members

    T& lat = P::operator[](0);
    T& lon = P::operator[](1);

    // -- Methods

    static PointLatLon make_normalised(T lat, T lon) {
        auto normal = [](double a, double minimum) {
            while (a < minimum) {
                a += 360.;
            }
            while (a >= minimum + 360.) {
                a -= 360.;
            }

            return a;
        };

        lat = normal(lat, -90.);

        if (lat > 90.) {
            lat -= 180.;
            lon += 180.;
        }

        return {lat, lat == -90. || lat == 90. ? 0. : normal(lon, 0.)};
    }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointLatLon& p) {
        return out << '{' << p.lat << ", " << p.lon << '}';
    }
};


}  // namespace grit::types
