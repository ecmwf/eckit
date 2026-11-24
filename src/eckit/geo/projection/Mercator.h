// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/**
 * @brief Mercator projection, as PROJ (merc)
 * @ref Map Projections: A Working Manual, John P. Snyder (1987)
 */
class Mercator : public Projection {
public:

    // -- Constructors

    explicit Mercator(double lon_0 = 0., double lat_ts = 0., Figure* = nullptr);
    explicit Mercator(const Spec&);

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

    /// Central meridian
    const double lon_0_;

    /// Latitude of true scale
    const double lat_ts_;

    const Figure::Conformal conformal_;

    /// Semi-major axis, scaled at the latitude of true scale
    const double k_;
};


}  // namespace eckit::geo::projection
