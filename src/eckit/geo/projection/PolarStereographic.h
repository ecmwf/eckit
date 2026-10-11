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
 * @brief Polar stereographic projection, as PROJ (stere) of lat_0 = 90 or -90
 * @ref Map Projections: A Working Manual, John P. Snyder (1987)
 */
class PolarStereographic : public Projection {
public:

    // -- Constructors

    /// The latitude of true scale lat_ts is the pole lat_0 if not given
    explicit PolarStereographic(double lat_0, double lat_ts, double lon_0 = 0., Figure* = nullptr);
    explicit PolarStereographic(const Spec&);

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

    const double lat_0_;
    const double lat_ts_;
    const double lon_0_;

    /// 1 for the North Pole, -1 for the South Pole
    const double sign_;

    const Conformal conformal_;

    /// Radius from the pole is k t, Snyder (21-33, 21-34)
    double k_;
};


}  // namespace eckit::geo::projection
