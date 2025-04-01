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


namespace eckit::geo {
class PointLonLatR;
}


namespace eckit::geo {


/**
 * @brief The PointLonLat class
 * @details A point on a geographic coordinate system, in (longitude, latitude) coordinates [degree]; They are fully
 * circular in space (also latitude), longitude is typically limited to [0, 360[ and latitude to [-90, 90], with
 * normalisation functions available, as well as conversion to and from radian-based coordinates.
 */
class PointLonLat final : protected std::array<double, 2> {
public:

    // -- Types

    using container_type = array;
    using container_type::value_type;

    // -- Constructors

    PointLonLat() : PointLonLat(0., 0.) {}

    PointLonLat(value_type lon, value_type lat) : container_type{lon, lat} {}

    PointLonLat(const PointLonLat& other) : container_type(other) {}

    PointLonLat(PointLonLat&& other) : container_type(other) {}

    // -- Destructor

    ~PointLonLat() = default;

    // -- Operators

    PointLonLat& operator=(const PointLonLat& other) {
        container_type::operator=(other);
        return *this;
    }

    PointLonLat& operator=(PointLonLat&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Members

    const value_type& lon = container_type::operator[](0);
    const value_type& lat = container_type::operator[](1);

    // -- Methods

    using container_type::data;

    bool pole(value_type eps = EPS) const;

    static size_t dimensions() { return DIMS; }

    static value_type normalise_angle_to_minimum(value_type, value_type minimum);
    static value_type normalise_angle_to_maximum(value_type, value_type maximum);

    static void assert_latitude_range(const PointLonLat&);

    [[nodiscard]] static PointLonLat make(value_type lon, value_type lat, value_type lon_minimum = 0.,
                                          value_type eps = EPS);

    [[nodiscard]] static PointLonLat make_from_lonlatr(value_type lonr, value_type latr, value_type lonr_minimum = 0.);

    PointLonLat antipode() const { return make(lon, lat + FLAT_ANGLE); }

    // -- Class members

    static constexpr size_t DIMS    = 2;
    static constexpr value_type EPS = 1e-9;

    static constexpr value_type FULL_ANGLE  = 360.;
    static constexpr value_type FLAT_ANGLE  = 180.;
    static constexpr value_type RIGHT_ANGLE = 90.;

    // -- Class methods

    static PointLonLat componentsMin(const PointLonLat& p, const PointLonLat& q);
    static PointLonLat componentsMax(const PointLonLat& p, const PointLonLat& q);

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointLonLat& p) {
        return out << '{' << p.lon << ", " << p.lat << '}';
    }

    friend PointLonLat operator-(const PointLonLat& p, const PointLonLat& q) { return {p.lon - q.lon, p.lat - q.lat}; }
    friend PointLonLat operator+(const PointLonLat& p, const PointLonLat& q) { return {p.lon + q.lon, p.lat + q.lat}; }
    friend PointLonLat operator*(const PointLonLat& p, value_type d) { return {p.lon * d, p.lat * d}; }

    friend bool operator<(const PointLonLat& p, const PointLonLat& q) {
        return static_cast<const container_type&>(p) < static_cast<const container_type&>(q);
    }
};


bool points_equal(const PointLonLat&, const PointLonLat&, PointLonLat::value_type eps = PointLonLat::EPS);


extern const PointLonLat NORTH_POLE;
extern const PointLonLat SOUTH_POLE;


}  // namespace eckit::geo
