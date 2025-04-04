/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include "eckit/geo/figure/OblateSpheroid.h"


namespace eckit::geo::figure {


/// Oblate spheroid parametrised with a geodetic datum
template <class DATUM>
class OblateSpheroidT : public Figure {
public:

    // -- Constructors

    OblateSpheroidT() = default;

    // -- Overridden methods

    double R() const override { return OblateSpheroid::R(DATUM::a, DATUM::b); }
    double a() const override { return DATUM::a; }
    double b() const override { return DATUM::b; }

    /// Surface area [L^2]
    double area() const override { return OblateSpheroid::_area(DATUM::a, DATUM::b); }

    /// Surface area between parallels and meridians [L^2]
    double area(const area::BoundingBox& bbox) override { return OblateSpheroid::_area(DATUM::a, DATUM::b, bbox); }
};


}  // namespace eckit::geo::figure
