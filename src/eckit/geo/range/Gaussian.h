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

#include "eckit/geo/Range.h"


namespace eckit::geo::range {


class Gaussian final : public Range {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit Gaussian(size_t N, double precision = 0.) :
        Gaussian(N, 90., -90., precision) {}

    Gaussian(size_t N, double a, double b, double precision = 0.);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    size_t N() const { return N_; }
    const std::vector<double>& values() const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const size_t N_;
    const double a_;
    const double b_;
    const double eps_;

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


}  // namespace eckit::geo::range