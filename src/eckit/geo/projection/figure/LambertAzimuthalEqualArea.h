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
#include "eckit/geo/util/sincos.h"


namespace eckit::geo::projection::figure {


class LambertAzimuthalEqualArea final : public ProjectionOnFigure {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit LambertAzimuthalEqualArea(const Spec&);
    LambertAzimuthalEqualArea(PointLonLat centre, PointLonLat first);

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

    [[nodiscard]] Spec* spec() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Types

    // -- Members

    const PointLonLat centre_;     // central meridian/standard parallel [degree]
    const PointLonLatR centre_r_;  // central meridian/standard parallel [radian]

    const PointLonLat first_;     // first point [degree]
    const PointLonLatR first_r_;  // first point [radian]

    const util::sincos_t phi0_;
    const util::sincos_t phi_;
    const util::sincos_t dlam_;

    // -- Methods
    // None

    // -- Overridden methods

    Point fwd(const Point& p) const override { return fwd(std::get<PointLonLat>(p)); }
    Point inv(const Point& q) const override { return inv(std::get<Point2>(q)); }

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::projection::figure
