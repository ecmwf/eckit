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

#include <memory>

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Calculate coordinates of a point on a sphere or spheroid, in [x, y, z]
class LonLatToXYZ final : public Projection {
public:
    // -- Exceptions
    // None

    // -- Constructors

    LonLatToXYZ(double a, double b);
    explicit LonLatToXYZ(double R);
    explicit LonLatToXYZ(const Spec&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    Point3 fwd(const PointLonLat& p) const { return (*impl_)(p); }
    PointLonLat inv(const Point3& q) const { return (*impl_)(q); }

    // -- Overridden methods

    Spec* spec() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Types

    struct Implementation {
        Implementation()          = default;
        virtual ~Implementation() = default;

        Implementation(const Implementation&) = delete;
        Implementation(Implementation&&)      = delete;
        void operator=(const Implementation&) = delete;
        void operator=(Implementation&&)      = delete;

        virtual Point3 operator()(const PointLonLat&) const = 0;
        virtual PointLonLat operator()(const Point3&) const = 0;
        virtual Spec* spec() const                          = 0;
    };

    // -- Members

    std::unique_ptr<Implementation> impl_;

    // -- Methods
    // None

    // -- Overridden methods

    Point fwd(const Point& p) const override { return (*impl_)(std::get<PointLonLat>(p)); }
    Point inv(const Point& q) const override { return (*impl_)(std::get<Point3>(q)); }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::projection
