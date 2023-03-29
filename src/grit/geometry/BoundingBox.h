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

#include <iosfwd>

#include "mir/util/Types.h"


struct grib_info;
namespace eckit {
class MD5;
}
namespace mir {
namespace api {
class MIRJob;
}
namespace param {
class MIRParametrisation;
}
}  // namespace mir


namespace mir::util {


class BoundingBox {
public:
    // -- Exceptions
    // None

    // -- Constructors

    BoundingBox();
    BoundingBox(const Latitude& north, const Longitude& west, const Latitude& south, const Longitude& east);
    BoundingBox(const param::MIRParametrisation&);
    BoundingBox(const BoundingBox&);

    // -- Destructor

    virtual ~BoundingBox();

    // -- Convertors
    // None

    // -- Operators

    BoundingBox& operator=(const BoundingBox&);

    bool operator==(const BoundingBox&) const;

    bool operator!=(const BoundingBox& other) const { return !operator==(other); }

    // -- Methods

    // DON'T IMPLEMENT SETTERS

    const Latitude& north() const { return north_; }

    const Longitude& west() const { return west_; }

    const Latitude& south() const { return south_; }

    const Longitude& east() const { return east_; }

    bool isPeriodicWestEast() const;

    bool contains(const PointLatLon&) const;

    bool contains(const Point2&) const;

    bool contains(const Latitude&, const Longitude&) const;

    bool contains(const BoundingBox&) const;

    bool intersects(BoundingBox&) const;

    bool empty() const;

    void fillGrib(grib_info&) const;

    void fillJob(api::MIRJob&) const;

    void hash(eckit::MD5&) const;

    void makeName(std::ostream&) const;

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

    virtual void print(std::ostream&) const;

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

    Latitude north_;
    Longitude west_;
    Latitude south_;
    Longitude east_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const BoundingBox& p) {
        p.print(s);
        return s;
    }
};


}  // namespace mir::util
