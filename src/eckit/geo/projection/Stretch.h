// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/**
 * @brief Latitude stretching (Schmidt transform) by a factor c > 0 ('factor'): tan(colat' / 2) = c tan(colat / 2),
 * attracting points to the North Pole (c < 1) or to the South Pole (c > 1)
 * @details Conformal, keeping longitudes and the poles, it refines the resolution around the North Pole by 1 / c
 * (F. Schmidt, 1977, "Variable fine mesh in spectral global models"); with a rotation placing that pole, it stretches
 * variable resolution grids (e.g. LFRic's stretch_mesh, see factor_from_equatorial_latitude). fwd stretches, inv
 * reverses (as the inverse factor); points are (lon, lat) or Cartesian.
 */
class Stretch : public Projection {
public:

    // -- Constructors

    explicit Stretch(double c, bool reverse = false);
    explicit Stretch(const Spec&);

    // -- Methods

    inline PointLonLat fwd(const PointLonLat& p) const { return stretch(p, C_, S_); }
    inline PointLonLat inv(const PointLonLat& q) const { return stretch(q, -C_, S_); }

    inline PointXYZ fwd(const PointXYZ& p) const { return stretch(p, C_, S_); }
    inline PointXYZ inv(const PointXYZ& q) const { return stretch(q, -C_, S_); }

    double factor() const { return c_; }

    // -- Overridden methods

    const std::string& type() const override;

    Point fwd(const Point&) const override;
    Point inv(const Point&) const override;

    // -- Class methods

    /// Stretching factor taking the equator to the given latitude [degree], c = sqrt((1 - sin(lat)) / (1 + sin(lat)))
    static double factor_from_equatorial_latitude(double);

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:

    // -- Members

    const double c_;
    const double C_;
    const double S_;

    // -- Class methods

    static PointLonLat stretch(const PointLonLat&, double C, double S);
    static PointXYZ stretch(const PointXYZ&, double C, double S);
};


}  // namespace eckit::geo::projection
