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

namespace eckit::geo {

class BoundingBox {
public:
    // -- Exceptions
    // None

    // -- Constructors

    BoundingBox(double north, double west, double south, double east);

    BoundingBox();

    BoundingBox(const BoundingBox&) = default;

    BoundingBox(BoundingBox&&) = default;

    // -- Destructor

    virtual ~BoundingBox() = default;

    // -- Convertors
    // None

    // -- Operators

    BoundingBox& operator=(const BoundingBox&) = default;

    BoundingBox& operator=(BoundingBox&&) = default;

    bool operator==(const BoundingBox&) const;

    bool operator!=(const BoundingBox& other) const { return !operator==(other); }

    // -- Methods

    double north() const { return north_; }

    double west() const { return west_; }

    double south() const { return south_; }

    double east() const { return east_; }

    bool isPeriodicWestEast() const;

    bool contains(double lat, double lon) const;

    bool contains(const BoundingBox&) const;

    bool intersects(BoundingBox&) const;

    bool empty() const;

    double area(double radius) const;

    // -- Overridden methods
    // None

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
    // None
};

}  // namespace eckit::geo
