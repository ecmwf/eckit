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
    // -- Constructors

    explicit LonLatToXY() = default;
    explicit LonLatToXY(const Spec&) {}

    // -- Methods

    inline Point2 fwd(const PointLonLat& p) const { return {p.lon, p.lat}; }
    inline PointLonLat inv(const Point2& q) const { return {q.X, q.Y}; }

    // -- Overridden methods

    [[nodiscard]] Spec* spec() const override;

private:
    // -- Overridden methods

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<Point2>(q)); }
};


}  // namespace eckit::geo::projection
