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
 * @brief AlbersEqualArea projection for the sphere
 */
class AlbersEqualArea : public Projection {
public:

    // -- Constructors

    explicit AlbersEqualArea(const Spec&);

    AlbersEqualArea(double lon_0, double lat_0, double lat_1, double lat_2, Figure* = nullptr);

    // -- Methods

    PointXY fwd(const PointLonLat&) const;
    PointLonLat inv(const PointXY&) const;

    // -- Overridden methods

    const std::string& type() const override;

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<PointXY>(q)); }

private:

    // -- Members

    const PointLonLat centre_;     // central meridian/parallel [degree]
    const PointLonLatR centre_r_;  // central meridian/parallel [radian]

    const double lat_1_;
    const double lat_2_;
    const double n_;
    const double R_;
    const double C_;
    const double rho0_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::projection
