// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>
#include <vector>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/projection/ToFromXYZ.h"


namespace eckit::geo::projection {


// (lon, lat, height) to [x, y, z], height along the normal of the figure (on a spheroid, the geodetic height)
// multiplied by height_scale_factor s >= 0; s other than 1 distorts the space, so distances weigh height differences by
// s (searches then prefer neighbours at similar heights) and horizontal distances by about (R + s h) / R
class LonLatHToXYZ : public ToFromXYZ {
public:

    // -- Constructors

    explicit LonLatHToXYZ(Figure* = nullptr, double height_scale_factor = 1.);

    explicit LonLatHToXYZ(const Spec&);

    // -- Methods

    using Projection::fwd;
    using Projection::inv;

    PointXYZ fwd(const PointLonLatH& p) const { return to_xyz(p.lonlat(), height_scale_factor_ * p.height()); }

    // on a sphere only, with s > 0 (otherwise height is not recoverable)
    PointLonLatH inv(const PointXYZ&) const;

    double height_scale_factor() const { return height_scale_factor_; }

    // -- Overridden methods

    const std::string& type() const override;

    Point fwd(const Point& p) const override { return fwd(std::get<PointLonLatH>(p)); }
    Point inv(const Point& q) const override { return inv(std::get<PointXYZ>(q)); }

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    std::vector<std::vector<double>> fwd_vector(const std::vector<double>& lon, const std::vector<double>& lat,
                                                const std::vector<double>& height) const override;

private:

    // -- Members

    const double height_scale_factor_;
};


}  // namespace eckit::geo::projection
