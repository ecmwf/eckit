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


class PolarStereographic : public ProjectionOnFigure {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit PolarStereographic(const Spec&);
    PolarStereographic(PointLonLat centre, PointLonLat first = {0, 0}, Figure* = nullptr);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    Point2 fwd(const PointLonLat& p) const;
    PointLonLat inv(const Point2& q) const;

    // -- Overridden methods

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<Point2>(q)); }

    // -- Class members
    // None

    // -- Class methods
    // None

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

    // -- Methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::projection
