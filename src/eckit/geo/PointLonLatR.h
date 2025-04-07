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
class PointLonLat;
}


namespace eckit::geo {


/**
 * @brief The PointLonLatR class
 * @details A point on a geographic coordinate system, in (longitude, latitude) coordinates [radian]. They are fully
 * circular in space (also latitude), longitude is typically limited to [0, 2 pi[ and latitude to [-pi, pi], with
 * normalisation functions available, as well as conversion to and from degree-based coordinates.
 */
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

    using container_type::data;

    bool pole(value_type eps = EPS) const;

    static size_t dimensions() { return DIMS; }

    static value_type normalise_angle_to_minimum(value_type, value_type minimum);
    static value_type normalise_angle_to_maximum(value_type, value_type maximum);

    [[nodiscard]] static PointLonLatR make(value_type lonr, value_type latr, value_type lonr_minimum = 0.,
                                           value_type eps = EPS);

    [[nodiscard]] static PointLonLatR make_from_lonlat(value_type lon, value_type lat, value_type lon_minimum = 0.);

    PointLonLatR antipode() const { return make(lonr, latr + FLAT_ANGLE); }

    // -- Class members

    static constexpr size_t DIMS    = 2;
    static constexpr value_type EPS = 1e-10;

    static constexpr value_type FULL_ANGLE  = 2. * M_PI;
    static constexpr value_type FLAT_ANGLE  = M_PI;
    static constexpr value_type RIGHT_ANGLE = M_PI_2;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointLonLatR& p) {
        return out << '{' << p.lonr << ", " << p.latr << '}';
    }
};


bool points_equal(const PointLonLatR&, const PointLonLatR&, PointLonLatR::value_type eps = PointLonLatR::EPS);


extern const PointLonLatR NORTH_POLE_R;
extern const PointLonLatR SOUTH_POLE_R;


}  // namespace eckit::geo
