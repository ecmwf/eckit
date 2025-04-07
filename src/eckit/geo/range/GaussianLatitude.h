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


class GaussianLatitude final : public Range {
public:

    // -- Constructors

    explicit GaussianLatitude(size_t N, bool increasing, double eps = 0.);

    // -- Methods

    size_t N() const { return N_; }

    // -- Overridden methods

    [[nodiscard]] Range* make_range_flipped() const override;
    [[nodiscard]] Range* make_range_cropped(double crop_a, double crop_b) const override;

    Fraction increment() const override;
    const std::vector<double>& values() const override;

private:

    // -- Constructors

    GaussianLatitude(size_t N, std::vector<double>&& values, double _eps) :
        Range(values.size(), values.front(), values.back(), _eps), N_(N), values_(values) {}

    // -- Members

    const size_t N_;
    std::vector<double> values_;
};


}  // namespace eckit::geo::range
