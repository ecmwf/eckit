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

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


class LonLatToXY final : public Projection {
public:
    // -- Exceptions
    // None

    // -- Constructors

    explicit LonLatToXY() = default;
    explicit LonLatToXY(const Spec&) {}

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    inline Point2 fwd(const PointLonLat& p) const { return {p.lon, p.lat}; }
    inline PointLonLat inv(const Point2& q) const { return {q.X, q.Y}; }

    // -- Overridden methods

    Spec* spec() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Types
    // None

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<Point2>(q)); }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::projection
