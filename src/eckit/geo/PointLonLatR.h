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


namespace eckit::geo {


class PointLonLatR final : protected std::array<double, 2> {
public:
    // -- Types

    using container_type = array;
    using container_type::value_type;

    // -- Constructors

    PointLonLatR() : PointLonLatR(0., 0.) {}

    PointLonLatR(value_type lon, value_type lat) : container_type{lon, lat} {}

    PointLonLatR(const PointLonLatR& other) : container_type(other) {}

    PointLonLatR(PointLonLatR&& other) : container_type(other) {}

    // -- Destructor

    ~PointLonLatR() = default;

    // -- Operators

    PointLonLatR& operator=(const PointLonLatR& other) {
        container_type::operator=(other);
        return *this;
    }

    PointLonLatR& operator=(PointLonLatR&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Members

    const value_type& lonr = container_type::operator[](0);
    const value_type& latr = container_type::operator[](1);

    // -- Methods

    static value_type normalise_angle_to_minimum(value_type, value_type minimum);

    static value_type normalise_angle_to_maximum(value_type, value_type maximum);

    [[nodiscard]] static PointLonLatR make(value_type lon, value_type lat, value_type lon_minimum = EQUATOR,
                                           value_type eps = EPS);

    PointLonLatR antipode() const { return make(lonr, latr + GLOBE / 2.); }

    // -- Class members

    static constexpr double GLOBE        = 2. * M_PI;
    static constexpr double GREENWICH    = 0.;
    static constexpr double ANTIMERIDIAN = -M_PI;
    static constexpr double EQUATOR      = 0.;
    static constexpr double NORTH_POLE   = M_PI_2;
    static constexpr double SOUTH_POLE   = -M_PI_2;

    static constexpr value_type EPS = 1e-10;

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointLonLatR& p) {
        return out << '{' << p.lonr << ", " << p.latr << '}';
    }
};


bool points_equal(const PointLonLatR&, const PointLonLatR&, PointLonLatR::value_type eps = PointLonLatR::EPS);


}  // namespace eckit::geo
