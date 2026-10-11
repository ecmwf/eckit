// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/projection/Conformal.h"


namespace eckit::geo::projection {


/**
 * @brief LambertConformalConic projection
 * @ref Map Projections: A Working Manual, John P. Snyder (1987)
 * @ref Wolfram MathWorld (http://mathworld.wolfram.com/LambertConformalConicProjection.html)
 */
class LambertConformalConic : public Projection {
public:

    // -- Constructors

    explicit LambertConformalConic(const Spec&);

    /// As PROJ (lcc), with lat_2 = lat_1 for one standard parallel
    explicit LambertConformalConic(double lat_1, double lat_2, double lon_0 = 0., double lat_0 = 0., Figure* = nullptr);

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

    /// Central meridian/longitude of natural origin, longitude of origin or longitude of false origin
    const double lon_0_;

    /// Latitude of natural origin, latitude of false origin or latitude of projection centre
    const double lat_0_;

    /// First standard parallel
    const double lat_1_;

    /// Second standard parallel
    const double lat_2_;

    const Conformal conformal_;

    /// Cone constant, scaled semi-major axis and radius of the parallel of origin, Snyder (15-1 to 15-3)
    double n_;
    double aF_;
    double rho0_;
};


}  // namespace eckit::geo::projection
