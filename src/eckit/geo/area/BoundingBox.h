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


namespace eckit::geo {
class Projection;
}


namespace eckit::geo::area {


class BoundingBox : public Area, protected std::array<double, 4> {
public:
    // -- Exceptions
    // None

    // -- Constructors

    explicit BoundingBox(const Spec&);

    BoundingBox(double north, double west, double south, double east);

    BoundingBox() :
        BoundingBox(make_global_prime()) {}

    BoundingBox(const BoundingBox& other) :
        array(other) {}

    BoundingBox(BoundingBox&& other) :
        array(other) {}

    // -- Destructor

    ~BoundingBox() override = default;

    // -- Convertors
    // None

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

    bool intersects(BoundingBox&) const;

    bool contains(double lat, double lon) const;
    bool contains(const BoundingBox&) const;
    bool empty() const;
    double area(double radius) const;

    static BoundingBox make(const BoundingBox&, const Projection&);
    static BoundingBox make_global_prime();
    static BoundingBox make_global_antiprime();

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Members

    double& north = operator[](0);
    double& west  = operator[](1);
    double& south = operator[](2);
    double& east  = operator[](3);

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const BoundingBox& bbox) {
        return os << "[" << bbox.north << "," << bbox.west << "," << bbox.south << "," << bbox.east << "]";
    }
};


}  // namespace eckit::geo::area
