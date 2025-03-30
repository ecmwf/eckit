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

#include "eckit/geo/Area.h"
#include "eckit/geo/PointXY.h"


namespace eckit::geo::area {


class BoundingBox2 : public Area, protected std::array<double, 4> {
public:

    // -- Types

    using container_type = array;
    using value_type     = container_type::value_type;

    // -- Constructors

    explicit BoundingBox2(const Spec&);

    BoundingBox2(value_type min_x, value_type min_y, value_type max_x, value_type max_y);

    BoundingBox2(const BoundingBox2& other) : Area(other), container_type(other) {}

    BoundingBox2(BoundingBox2&& other) : Area(other), container_type(other) {}

    // -- Destructor

    ~BoundingBox2() override = default;

    // -- Operators

    BoundingBox2& operator=(const BoundingBox2& other) {
        container_type::operator=(other);
        return *this;
    }

    BoundingBox2& operator=(BoundingBox2&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Methods

    container_type deconstruct() const { return {min_x, min_y, max_x, max_y}; }

    bool contains(const Point&) const override;
    bool contains(const BoundingBox2&) const;
    bool empty() const;

    // -- Overridden methods

    const std::string& type() const override;

    void fill_spec(spec::Custom&) const override;

    // -- Class methods

    [[nodiscard]] static BoundingBox2* make_from_spec(const Spec&);

    // -- Members

    const value_type& min_x = operator[](0);
    const value_type& min_y = operator[](1);
    const value_type& max_x = operator[](2);
    const value_type& max_y = operator[](3);
};


}  // namespace eckit::geo::area
