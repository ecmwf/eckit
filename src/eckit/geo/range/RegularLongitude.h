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


class RegularLongitude final : public Range {
public:
    // -- Constructors

    RegularLongitude(size_t n, double a, double b, double eps = 0.);

    // -- Methods
    bool periodic() const { return periodic_ != NonPeriodic; }

    // -- Overridden methods

    Range* flip() const override;
    Range* crop(double crop_a, double crop_b) const override;
    const std::vector<double>& values() const override;

private:
    // -- Types

    enum Periodic
    {
        NonPeriodic = 0,
        PeriodicNoEndPoint,
        PeriodicNoStartPoint
    };

    // -- Constructors

    RegularLongitude(size_t n, double a, double b, Periodic periodic, double eps) :
        Range(n, a, b, eps), periodic_(periodic) {}

    // -- Members

    Periodic periodic_;
    std::vector<double> values_;

    // -- Methods

    Fraction increment() const;
};


}  // namespace eckit::geo::range
