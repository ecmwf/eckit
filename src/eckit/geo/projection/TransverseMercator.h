// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <array>
#include <string>
#include <utility>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/**
 * @brief Transverse Mercator projection, as PROJ (tmerc)
 * @details Krüger series to the sixth order in the third flattening, accurate to a few nanometres within 3900 km of the
 * central meridian (and exact on the sphere). The false easting/northing are not part of the projection (as for the
 * other projections), but of the grid.
 * @ref Karney, C.F.F. (2011). Transverse Mercator with an accuracy of a few nanometers. J. Geodesy 85(8), 475-485
 */
class TransverseMercator : public Projection {
public:

    // -- Constructors

    explicit TransverseMercator(double lon_0 = 0., double lat_0 = 0., double k_0 = 1., Figure* = nullptr);
    explicit TransverseMercator(const Spec&);

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

    using Series = std::array<double, 6>;

    // -- Members

    /// Central meridian
    const double lon_0_;

    /// Latitude of origin
    const double lat_0_;

    /// Scale factor on the central meridian
    const double k_0_;

    const Figure::Conformal conformal_;

    /// Krüger series coefficients, from (alpha) and to (beta) the conformal sphere
    Series alpha_;
    Series beta_;

    /// Rectifying radius, scaled by k_0
    double kA_;

    /// Northing of the latitude of origin
    double y_0_;

    // -- Class methods

    /// Krüger series: (xi, eta) on the ellipsoid from (xi', eta') on the conformal sphere (alpha, sign +1), or back
    /// (beta, sign -1)
    static std::pair<double, double> krueger(double xi, double eta, const Series&, double sign);
};


}  // namespace eckit::geo::projection
