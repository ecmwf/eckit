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

#include "eckit/geo/Figure.h"


namespace eckit::geo::figure {


class Sphere final : public Figure {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit Sphere(double R);
    explicit Sphere(const Spec&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    double R() const override { return R_; }
    double a() const override { return R_; }
    double b() const override { return R_; }


    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const double R_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::figure
