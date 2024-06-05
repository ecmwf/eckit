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


class Stretch final : public Projection {
public:
    // -- Constructors

    explicit Stretch(double c);
    explicit Stretch(const Spec&);

    // -- Methods

    PointLonLat fwd(const PointLonLat& p) const { return PointLonLat::make(p.lon, stretch(p.lat, 1. / c_)); }
    PointLonLat inv(const PointLonLat& p) const { return PointLonLat::make(p.lon, stretch(p.lat, c_)); }

    // -- Overridden methods

    void spec(spec::Custom&) const override;

private:
    // -- Members

    double c_;

    // -- Methods

    static double stretch(double a, double c);

    // -- Overridden methods

    Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    Point inv(const Point& q) const override { return inv(std::get<PointLonLat>(q)); }
};


}  // namespace eckit::geo::projection