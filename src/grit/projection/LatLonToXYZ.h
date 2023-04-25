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

#include "grit/Projection.h"


namespace grit::projection {


/// Compute coordinates of a point on a sphere or spheroid, in [x, y, z]
class LatLonToXYZ final : public Projection {
public:
    // -- Exceptions
    // None

    // -- Constructors

    LatLonToXYZ(double a, double b);
    explicit LatLonToXYZ(double R);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    PointXYZ fwd(const PointLatLon& p) const { return (*impl_)(p); }
    PointLatLon inv(const PointXYZ& q) const { return (*impl_)(q); }

    // -- Overridden methods
    // None

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

        virtual PointXYZ operator()(const PointLatLon&) const = 0;
        virtual PointLatLon operator()(const PointXYZ&) const = 0;
    };

    // -- Members

    std::unique_ptr<Implementation> impl_;

    // -- Methods
    // None

    // -- Overridden methods

    Point fwd(const Point& p) const override { return (*impl_)(std::get<PointLatLon>(p)); }
    Point inv(const Point& q) const override { return (*impl_)(std::get<PointXYZ>(q)); }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace grit::projection
