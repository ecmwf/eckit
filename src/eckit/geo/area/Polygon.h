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

#include <vector>

#include "eckit/geo/Area.h"
#include "eckit/geo/PointXY.h"
#include "eckit/geo/polygon/Polygon.h"


namespace eckit::geo::area {


class Polygon : public Area, protected std::vector<polygon::Polygon> {
public:

    // -- Types

    using container_type = vector;
    using value_type     = container_type::value_type;

    // -- Constructors

    explicit Polygon(const Spec&);

    Polygon(const Polygon& other) : Area(other), container_type(other) {}
    Polygon(Polygon&& other) : Area(other), container_type(other) {}

    Polygon(const container_type& cont) : vector(cont) {}
    Polygon(container_type&& cont) : vector(cont) {}

    // -- Destructor

    ~Polygon() override = default;

    // -- Operators

    Polygon& operator=(const Polygon& other) {
        container_type::operator=(other);
        return *this;
    }

    Polygon& operator=(Polygon&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Methods

    using vector::empty;

    // -- Overridden methods

    const std::string& type() const override;
    void fill_spec(spec::Custom&) const override;

    bool intersects(BoundingBox&) const override;
    bool contains(const Point&) const override;
    double area() const override;

    // -- Class methods

    [[nodiscard]] static Polygon* make_from_spec(const Spec&);
};


}  // namespace eckit::geo::area
