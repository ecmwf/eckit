// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/**
 * @brief Geostationary satellite view (similar to PROJ geos, not equivalent); points not visible project to NaN
 * @ref LRIT/HRIT Global Specification (CGMS 03, Issue 2.6, 12.08.1999)
 */
class SpaceView : public Projection {
public:

    // -- Constructors

    /// h: height of the satellite above the surface [m], sweep: "x" or "y" (axis of the outer rotation)
    explicit SpaceView(double h, double lon_0 = 0., const std::string& sweep = "y", Figure* = nullptr);
    explicit SpaceView(const Spec&);

    // -- Methods

    PointXY fwd(const PointLonLat&) const;
    PointLonLat inv(const PointXY&) const;

    // -- Overridden methods

    const std::string& type() const override;

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<PointXY>(q)); }

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:

    // -- Members

    const double h_;
    const double lon_0_;
    const bool sweep_x_;

    /// Distance of the satellite from the centre, and polar radius [semi-major axis]
    const double radius_g_;
    const double radius_p_;
};


}  // namespace eckit::geo::projection
