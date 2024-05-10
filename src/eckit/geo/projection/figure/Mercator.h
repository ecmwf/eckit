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

#include <memory>

#include "eckit/geo/Figure.h"
#include "eckit/geo/projection/ProjectionOnFigure.h"


namespace eckit::geo::projection::figure {


/// Calculate coordinates of a point on a rotated sphere given new location of South Pole (vector) and angle
class Mercator final : public ProjectionOnFigure {
public:
    // -- Constructors

    Mercator(double meridian, double parallel, Figure*, PointLonLat first = {0, 0});

    explicit Mercator(const Spec&);

    // -- Methods

    Point2 fwd(const PointLonLat& p) const;
    PointLonLat inv(const Point2& q) const;

    // -- Overridden methods

    [[nodiscard]] Spec* spec() const override;

private:
    // -- Members

    const double meridian_;  // angle [degree] between Eastward direction and the Equator, range [0, 90]
    const double parallel_;  // latitude [degree] of projection intersecting ellipsoid

    const std::unique_ptr<Figure> figure_;
    PointLonLat first_;

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

    // -- Overridden methods

    Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    Point inv(const Point& q) const override { return inv(std::get<Point2>(q)); }
};


}  // namespace eckit::geo::projection::figure
