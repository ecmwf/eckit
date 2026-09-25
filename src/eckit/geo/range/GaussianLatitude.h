// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/geo/Range.h"


namespace eckit::geo::range {


class GaussianLatitude final : public Range {
public:

    // -- Constructors

    explicit GaussianLatitude(size_t N, bool increasing);

    // -- Methods

    size_t N() const { return N_; }

    // -- Overridden methods

    [[nodiscard]] GaussianLatitude* make_cropped_range(double crop_a, double crop_b) const override;
    [[nodiscard]] const std::vector<double>& values() const override;

    size_t size() const override { return values_.size(); }
    double a() const override { return values_.front(); }
    double b() const override { return values_.back(); }

    bool includesNorthPole() const override;
    bool includesSouthPole() const override;

private:

    // -- Constructors

    GaussianLatitude(size_t N, std::vector<double>&& values) : N_(N), values_(values) {}

    // -- Members

    const size_t N_;
    const std::vector<double> values_;
};


}  // namespace eckit::geo::range
