// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <array>
#include <cstddef>
#include <ostream>

#include "eckit/geo/PointLonLat.h"


namespace eckit::geo {


/**
 * @brief The PointLonLatH class
 * @details A point on a geographic coordinate system at a height, in (longitude, latitude) coordinates [degree] as
 * PointLonLat, and height [m] along the normal of a figure (see projection::LonLatHToXYZ). Negative heights are depths
 * below the figure.
 */
class PointLonLatH final : protected std::array<double, 3> {
public:

    // -- Types

    using container_type = array;
    using container_type::value_type;

    // -- Constructors

    PointLonLatH() : PointLonLatH(0., 0., 0.) {}

    PointLonLatH(value_type lon, value_type lat, value_type height) : container_type{lon, lat, height} {}

    PointLonLatH(const PointLonLat& p, value_type height) : PointLonLatH(p.lon(), p.lat(), height) {}

    PointLonLatH(const PointLonLatH& other) : container_type(other) {}

    PointLonLatH(PointLonLatH&& other) : container_type(other) {}

    // -- Destructor

    ~PointLonLatH() = default;

    // -- Operators

    PointLonLatH& operator=(const PointLonLatH& other) {
        container_type::operator=(other);
        return *this;
    }

    PointLonLatH& operator=(PointLonLatH&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Members

    value_type lon() const { return container_type::operator[](0); }
    value_type lat() const { return container_type::operator[](1); }
    value_type height() const { return container_type::operator[](2); }

    // -- Methods

    using container_type::data;

    PointLonLat lonlat() const { return {lon(), lat()}; }

    static size_t dimensions() { return DIMS; }

    /// As PointLonLat::make, keeping the height
    [[nodiscard]] static PointLonLatH make(value_type lon, value_type lat, value_type height,
                                           value_type lon_minimum = 0., value_type eps = EPS);

    // -- Class members

    static constexpr size_t DIMS    = 3;
    static constexpr value_type EPS = PointLonLat::EPS;

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const PointLonLatH& p) {
        return out << '{' << p.lon() << ", " << p.lat() << ", " << p.height() << '}';
    }
};


bool points_equal(const PointLonLatH&, const PointLonLatH&, PointLonLatH::value_type eps = PointLonLatH::EPS);


}  // namespace eckit::geo
