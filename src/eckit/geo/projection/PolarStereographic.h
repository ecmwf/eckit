// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


class PolarStereographic : public Projection {
public:

    // -- Constructors

    explicit PolarStereographic(const Spec&);
    PolarStereographic(PointLonLat centre, PointLonLat first, Figure* = nullptr);

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

    const PointLonLat centre_;     // projection centre [degree]
    const PointLonLatR centre_r_;  // projection centre [radian]

    const PointLonLat first_;     // first point [degree]
    const PointLonLatR first_r_;  // first point [radian]

    const double sign_;
    const double F_;
    double x0_;
    double y0_;
};


}  // namespace eckit::geo::projection
