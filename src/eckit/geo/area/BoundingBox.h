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
#include <memory>

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

    container_type deconstruct() const { return {north(), west(), south(), east()}; }

    bool global() const;
    bool periodic() const;

    bool contains(const BoundingBox&) const;
    bool empty() const;

    /// Surface area, assuming Earth [L^2]
    double area() const override;

    // -- Overridden methods

    const std::string& type() const override;

    void fill_spec(spec::Custom&) const override;

    bool intersects(BoundingBox&) const override;
    bool contains(const Point&) const override;

    // -- Class methods

    static const BoundingBox& bounding_box_default();

    [[nodiscard]] static std::unique_ptr<BoundingBox> make_global_prime();
    [[nodiscard]] static std::unique_ptr<BoundingBox> make_global_antiprime();

    [[nodiscard]] static std::unique_ptr<BoundingBox> make_from_area(value_type n, value_type w, value_type s,
                                                                     value_type e);
    [[nodiscard]] static std::unique_ptr<BoundingBox> make_from_spec(const Spec&);

    [[nodiscard]] static std::unique_ptr<BoundingBox> make_from_projection(PointXY min, PointXY max, const Projection&);
    [[nodiscard]] static std::unique_ptr<BoundingBox> make_from_projection(PointLonLat min, PointLonLat max,
                                                                           const projection::Rotation&);

    // -- Members

    value_type north() const { return operator[](0); }
    value_type west() const { return operator[](1); }
    value_type south() const { return operator[](2); }
    value_type east() const { return operator[](3); }
};


}  // namespace eckit::geo::area
