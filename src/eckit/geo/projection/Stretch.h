// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


class Stretch : public Projection {
public:

    // -- Constructors

    explicit Stretch(double c);
    explicit Stretch(const Spec&);

    // -- Methods

    inline PointLonLat fwd(const PointLonLat& p) const { return PointLonLat::make(p.lon(), stretch(p.lat(), 1. / c_)); }
    inline PointLonLat inv(const PointLonLat& p) const { return PointLonLat::make(p.lon(), stretch(p.lat(), c_)); }

    // -- Overridden methods

    const std::string& type() const override;

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<PointLonLat>(q)); }

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:

    // -- Members

    double c_;

    // -- Methods

    static double stretch(double a, double c);
};


}  // namespace eckit::geo::projection
