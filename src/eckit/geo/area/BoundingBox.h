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
#include "eckit/geo/Point.h"


namespace eckit::geo {
class Figure;
class Projection;
}  // namespace eckit::geo


namespace eckit::geo::area {


class BoundingBox : public Area, protected std::array<double, 4> {
public:
    // -- Constructors

    explicit BoundingBox(const Spec&);

    BoundingBox(double north, double west, double south, double east);

    BoundingBox();

    BoundingBox(const BoundingBox& other) : array(other) {}

    BoundingBox(BoundingBox&& other) : array(other) {}

    // -- Destructor

    ~BoundingBox() override = default;

    // -- Operators

    bool operator==(const BoundingBox&) const;
    bool operator!=(const BoundingBox& other) const { return !operator==(other); }

    BoundingBox& operator=(const BoundingBox& other) {
        array::operator=(other);
        return *this;
    }

    BoundingBox& operator=(BoundingBox&& other) {
        array::operator=(other);
        return *this;
    }

    // -- Methods

    std::array<double, 4> deconstruct() const { return {north, west, south, east}; }

    bool isPeriodicWestEast() const;
    bool containsNorthPole() const;
    bool containsSouthPole() const;

    bool intersects(BoundingBox&) const;

    bool contains(const PointLonLat&) const;
    bool contains(const BoundingBox&) const;
    bool empty() const;

    /// @brief Calculate the bounding box prescribed area ([[R]**2])
    double area(const Figure&) const;

    // -- Overridden methods

    void spec(spec::Custom&) const override;

    // -- Class methods

    static BoundingBox calculate(const BoundingBox&, const Projection&);

    static BoundingBox* make_global_prime();
    static BoundingBox* make_global_antiprime();
    static BoundingBox* make_from_spec(const Spec&);

    // -- Members

    const double& north = operator[](0);
    const double& west  = operator[](1);
    const double& south = operator[](2);
    const double& east  = operator[](3);

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const BoundingBox& bbox) {
        return os << "[" << bbox.north << "," << bbox.west << "," << bbox.south << "," << bbox.east << "]";
    }
};


}  // namespace eckit::geo::area
