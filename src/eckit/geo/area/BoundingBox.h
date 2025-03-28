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

#include "eckit/geo/Area.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/PointXY.h"


namespace eckit::geo {
namespace area {
class BoundingBox;
bool bounding_box_equal(const BoundingBox&, const BoundingBox&);
}  // namespace area
class Projection;
namespace projection {
class Rotation;
}
}  // namespace eckit::geo


namespace eckit::geo::area {


class BoundingBox : public Area, protected std::array<double, 4> {
public:

    // -- Types

    using container_type = array;
    using value_type     = container_type::value_type;

    // -- Constructors

    explicit BoundingBox(const Spec&);

    BoundingBox(value_type north, value_type west, value_type south, value_type east);

    BoundingBox();

    BoundingBox(const BoundingBox& other) : Area(other), container_type(other) {}

    BoundingBox(BoundingBox&& other) : Area(other), container_type(other) {}

    // -- Destructor

    ~BoundingBox() override = default;

    // -- Operators

    bool operator==(const BoundingBox& other) const { return bounding_box_equal(*this, other); }
    bool operator!=(const BoundingBox& other) const { return !operator==(other); }

    BoundingBox& operator=(const BoundingBox& other) {
        container_type::operator=(other);
        return *this;
    }

    BoundingBox& operator=(BoundingBox&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Methods

    container_type deconstruct() const { return {north, west, south, east}; }

    bool global() const;
    bool periodic() const;

    bool contains(const BoundingBox&) const;
    bool empty() const;

    // -- Overridden methods

    const std::string& type() const override;

    void fill_spec(spec::Custom&) const override;

    bool intersects(BoundingBox&) const override;
    bool contains(const Point&) const override;

    // -- Class methods

    [[nodiscard]] static BoundingBox* make_global_prime();
    [[nodiscard]] static BoundingBox* make_global_antiprime();
    [[nodiscard]] static BoundingBox* make_from_area(value_type n, value_type w, value_type s, value_type e);

    [[nodiscard]] static BoundingBox* make_from_spec(const Spec&);

    [[nodiscard]] static BoundingBox* make_from_projection(PointXY min, PointXY max, const Projection&);
    [[nodiscard]] static BoundingBox* make_from_projection(PointLonLat min, PointLonLat max,
                                                           const projection::Rotation&);

    // -- Members

    const value_type& north = operator[](0);
    const value_type& west  = operator[](1);
    const value_type& south = operator[](2);
    const value_type& east  = operator[](3);
};


extern const PointLonLat::value_type BOUNDING_BOX_NORMALISE_WEST;
extern const BoundingBox BOUNDING_BOX_DEFAULT;


}  // namespace eckit::geo::area
