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


/// Calculate coordinates of a point on a rotated sphere given new location of South Pole (vector) and angle
class Mercator : public ProjectionOnFigure {
public:

    // -- Constructors

    explicit Mercator(PointLonLat centre, PointLonLat first = {0, 0}, Figure* = nullptr);

    explicit Mercator(const Spec&);

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

    const PointLonLat centre_;  // angle [degree] between Eastward direction and the Equator, range [0, 90], latitude
                                // [degree] of projection intersecting ellipsoid
    const PointLonLat first_;

    const double eps_;
    const size_t max_iter_;

    double lam0_;
    double x0_;
    double y0_;
    double e_;
    double m_;
    double w_;

    // -- Methods

    double calculate_phi(double t) const;
};


}  // namespace eckit::geo::projection
