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
#include "eckit/types/Fraction.h"


namespace eckit::geo::range {


class Regular final : public Range {
public:

    // -- Methods

    [[nodiscard]] static Regular* make_xy_range(double inc, double a, double b, double ref = 0);
    [[nodiscard]] static Regular* make_latitude_range(double inc, double a, double b, double ref = 0);
    [[nodiscard]] static Regular* make_longitude_range(double inc, double a, double b, double ref = 0);

    [[nodiscard]] Range* make_cropped_range(double crop_a, double crop_b) const override;

    // -- Overridden methods

    bool periodic() const override { return periodic_; }
    Fraction increment() const override { return increment_; }
    const std::vector<double>& values() const override;

private:

    // -- Constructors

    Regular(Fraction inc, Fraction a, Fraction b, bool periodic);

    // -- Members

    const Fraction increment_;
    const Fraction a_;
    const Fraction b_;
    const bool periodic_;
};


}  // namespace eckit::geo::range
