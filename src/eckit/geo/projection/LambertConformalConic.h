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


/**
 * @brief LambertConformalConic projection
 * @ref Map Projections: A Working Manual, John P. Snyder (1987)
 * @ref Wolfram MathWorld (http://mathworld.wolfram.com/LambertConformalConicProjection.html)
 */
class LambertConformalConic : public Projection {
public:

    // -- Constructors

    explicit LambertConformalConic(const Spec&);

    explicit LambertConformalConic(double lat_1, double lon_0 = 0., double lat_0 = 0., double lat_2 = 0.,
                                   Figure* = nullptr);

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

    double n_;
    double f_;
    double rho0_bare_;
};


}  // namespace eckit::geo::projection
