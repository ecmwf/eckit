// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Latitude stretching by a factor: fwd from unstretched to the stretched sphere, inv the reverse (as by the inverse
/// factor)
class Stretch : public Projection {
public:

    // -- Constructors

    explicit Stretch(double c, bool reverse = false);
    explicit Stretch(const Spec&);

    // -- Methods

    inline PointLonLat fwd(const PointLonLat& p) const { return PointLonLat::make(p.lon(), stretch(p.lat(), c_)); }
    inline PointLonLat inv(const PointLonLat& q) const { return PointLonLat::make(q.lon(), stretch(q.lat(), 1. / c_)); }

    // -- Overridden methods

    const std::string& type() const override;

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<PointLonLat>(q)); }

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:

    // -- Members

    const double c_;

    // -- Methods

    static double stretch(double a, double c);
};


}  // namespace eckit::geo::projection
