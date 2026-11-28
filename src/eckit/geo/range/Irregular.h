// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/geo/Range.h"


namespace eckit::geo::range {


/// Irregular range of values, strictly monotonic (increasing or decreasing)
class Irregular : public Range {
public:

    // -- Constructors

    explicit Irregular(const std::vector<double>&);

    // -- Methods

    bool increasing() const { return values_.size() < 2 || values_.front() < values_.back(); }

    // -- Overridden methods

    [[nodiscard]] Irregular* make_cropped_range(double crop_a, double crop_b) const override;
    [[nodiscard]] const std::vector<double>& values() const override { return values_; }

    size_t size() const override { return values_.size(); }
    double a() const override { return values_.front(); }
    double b() const override { return values_.back(); }

private:

    // -- Members

    std::vector<double> values_;
};


}  // namespace eckit::geo::range
