// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/geo/Range.h"


namespace eckit::geo::range {


/// Regular range of (x or y) values, from a by steps of inc, to b (included, to a small fraction of the step)
class RegularXY : public Range {
public:

    // -- Constructors

    RegularXY(double inc, double a, double b);

    // -- Methods

    double inc() const { return inc_; }

    // -- Overridden methods

    [[nodiscard]] RegularXY* make_cropped_range(double crop_a, double crop_b) const override;
    [[nodiscard]] const std::vector<double>& values() const override { return values_; }

    size_t size() const override { return values_.size(); }
    double a() const override { return values_.front(); }
    double b() const override { return values_.back(); }

    Fraction increment() const override;

private:

    // -- Members

    double inc_;
    std::vector<double> values_;
};


}  // namespace eckit::geo::range
