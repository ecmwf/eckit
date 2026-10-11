// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/**
 * @brief Lambert azimuthal equal-area projection, as PROJ (laea)
 * @ref Map Projections: A Working Manual, John P. Snyder (1987)
 */
class LambertAzimuthalEqualArea : public Projection {
public:

    // -- Constructors

    explicit LambertAzimuthalEqualArea(double lat_0 = 0., double lon_0 = 0., Figure* = nullptr);
    explicit LambertAzimuthalEqualArea(const Spec&);

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

    // -- Types

    enum class Aspect {
        NORTH_POLE,
        SOUTH_POLE,
        OBLIQUE,  // and equatorial
    };

    // -- Members

    const double lat_0_;
    const double lon_0_;
    const double a_;
    const double e_;
    const Aspect aspect_;

    /// Authalic: q at the pole, radius, and sine/cosine of the latitude of the origin, Snyder (24-11 to 24-14)
    double qp_;
    double rq_;
    double sinb_0_;
    double cosb_0_;
    double d_;

    // -- Methods

    /// Snyder (3-12)
    double q(double sinphi) const;

    /// The latitude of the authalic latitude beta, Snyder (3-16)
    double phi(double beta) const;
};


}  // namespace eckit::geo::projection
