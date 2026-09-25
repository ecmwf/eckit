// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>
#include <vector>

#include "eckit/geo/Area.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/polygon/Polygon.h"


namespace eckit::geo::area {


class Polygon : public Area, protected std::vector<polygon::Polygon> {
public:

    // -- Types

    using container_type = vector;
    using value_type     = container_type::value_type;

    // -- Constructors

    explicit Polygon(const Area::Spec&);

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

    bool intersects(BoundingBox&) const override;
    bool contains(const Point&) const override;
    double area() const override;

    // -- Class methods

    [[nodiscard]] static Polygon* make_from_spec(const Area::Spec&);

private:

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::area
