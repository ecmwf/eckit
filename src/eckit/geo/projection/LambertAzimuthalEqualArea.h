// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/Projection.h"
#include "eckit/geo/util/sincos.h"


namespace eckit::geo::projection {


class LambertAzimuthalEqualArea : public Projection {
public:

    // -- Constructors

    explicit LambertAzimuthalEqualArea(const Spec&);
    LambertAzimuthalEqualArea(PointLonLat centre, PointLonLat first, Figure* = nullptr);

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

    const PointLonLat centre_;     // central meridian/standard parallel [degree]
    const PointLonLatR centre_r_;  // central meridian/standard parallel [radian]

    const PointLonLat first_;     // first point [degree]
    const PointLonLatR first_r_;  // first point [radian]

    const util::sincos_t phi0_;
    const util::sincos_t phi_;
    const util::sincos_t dlam_;
};


}  // namespace eckit::geo::projection
