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

#include "eckit/geo/projection/ProjectionOnFigure.h"


namespace eckit::geo::projection {


/**
 * @brief LambertConformalConic projection
 * @ref Map Projections: A Working Manual, John P. Snyder (1987)
 * @ref Wolfram MathWorld (http://mathworld.wolfram.com/LambertConformalConicProjection.html)
 */
class LambertConformalConic : public ProjectionOnFigure {
public:

    // -- Constructors

    explicit LambertConformalConic(const Spec&);

    LambertConformalConic(PointLonLat centre, PointLonLat first, double lat_1, double lat_2);
    LambertConformalConic(PointLonLat centre, PointLonLat first, double lat_1) :
        LambertConformalConic(centre, first, lat_1, lat_1) {}

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

    const PointLonLat centre_;     // central meridian/parallel [degree]
    const PointLonLatR centre_r_;  // central meridian/parallel [radian]

    const PointLonLat first_;     // first point [degree]
    const PointLonLatR first_r_;  // first point [radian]

    const double lat_1_;
    const double lat_1_r_;
    const double lat_2_;
    const double lat_2_r_;

    double n_;
    double f_;
    double rho0_bare_;
};


}  // namespace eckit::geo::projection
