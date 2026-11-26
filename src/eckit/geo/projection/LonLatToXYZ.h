// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>
#include <vector>

#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


/// Calculate coordinates of a point on a sphere or spheroid, in [x, y, z]
class LonLatToXYZ : public Projection {
public:

    // -- Constructors

    explicit LonLatToXYZ(Figure* = nullptr);

    explicit LonLatToXYZ(double R);
    explicit LonLatToXYZ(double a, double b);

    explicit LonLatToXYZ(const Spec&);

    // -- Methods

    using Projection::fwd;
    using Projection::inv;

    PointXYZ fwd(const PointLonLat&) const;
    PointLonLat inv(const PointXYZ&) const;

    // -- Overridden methods

    const std::string& type() const override;

    Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    Point inv(const Point& q) const override { return inv(std::get<PointXYZ>(q)); }

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    std::vector<std::vector<double>> fwd_vector(const std::vector<double>& lon, const std::vector<double>& lat,
                                                const std::vector<double>&) const override;

private:

    // -- Members

    const double a_;
    const double b_;
    const bool spherical_;
};


}  // namespace eckit::geo::projection
