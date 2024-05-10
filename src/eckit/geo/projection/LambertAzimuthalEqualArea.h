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

#include <array>

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


class LambertAzimuthalEqualArea final : public Projection {
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

    struct PointLonLatR final : protected std::array<double, 2> {
        PointLonLatR(value_type lonr, value_type latr);
        explicit PointLonLatR(const PointLonLat&);

        const value_type& lonr = array::operator[](0);
        const value_type& latr = array::operator[](1);
    };

    struct sincos_t final : std::array<double, 2> {
        explicit sincos_t(value_type r);

        const value_type& sin = array::operator[](0);
        const value_type& cos = array::operator[](1);
    };

    // -- Members

    const PointLonLat centre_;     // central longitude/standard parallel [degree]
    const PointLonLatR centre_r_;  // central_longitude/standard parallel [radian]

    const PointLonLat first_;     // first point [degree]
    const PointLonLatR first_r_;  // first point [radian]

    const sincos_t phi0_;
    const sincos_t phi_;
    const sincos_t dlam_;
    const double R_;

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


}  // namespace eckit::geo::projection
