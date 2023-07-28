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

#include <ostream>
#include <tuple>

#include "eckit/geometry/Area.h"


namespace eckit {
namespace geometry {
class Projection;
}
}  // namespace eckit


namespace eckit::geometry::area {


class BoundingBox : public Area {
public:
    // -- Exceptions
    // None

    // -- Constructors

    explicit BoundingBox(const Configuration&);

    BoundingBox(double north, double west, double south, double east);
    BoundingBox();

    BoundingBox(const BoundingBox&) = default;
    BoundingBox(BoundingBox&&)      = default;

    // -- Destructor

    virtual ~BoundingBox() = default;

    // -- Convertors
    // None

    // -- Operators

    BoundingBox& operator=(const BoundingBox&) = default;
    BoundingBox& operator=(BoundingBox&&)      = default;
    bool operator==(const BoundingBox&) const;
    bool operator!=(const BoundingBox& other) const { return !operator==(other); }

    // -- Methods

    std::tuple<double, double, double, double> deconstruct() const { return {north_, west_, south_, east_}; }

    double north() const { return north_; }
    double west() const { return west_; }
    double south() const { return south_; }
    double east() const { return east_; }

    bool isPeriodicWestEast() const;

    bool intersects(BoundingBox&) const;

    bool contains(double lat, double lon) const;
    bool contains(const BoundingBox&) const;
    bool empty() const;
    double area(double radius) const;

    static BoundingBox make(const BoundingBox&, const Projection&);

    // -- Overridden methods

    area::BoundingBox bbox() const override { return *this; }

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None

private:
    // -- Members

    double north_;
    double west_;
    double south_;
    double east_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& out, const BoundingBox& p) {
        return out << '{' << p.north_ << ", " << p.west_ << ", " << p.south_ << ", " << p.east_ << '}';
    }
};


}  // namespace eckit::geometry::area
