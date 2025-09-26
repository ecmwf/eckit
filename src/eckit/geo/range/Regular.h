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

    [[nodiscard]] static Range* make_regular_xy_range(double inc, double a, double b, double ref = 0);
    [[nodiscard]] static Range* make_regular_latitude_range(double inc, double a, double b, double ref = 0);
    [[nodiscard]] static Range* make_regular_longitude_range(double inc, double a, double b, double ref = 0);

    [[nodiscard]] Range* make_range_cropped(double a, double b) const override;

    Fraction increment() const override;

    // -- Overridden methods

    const std::vector<double>& values() const override;

private:

    // -- Constructors

    Regular(double a, double b, size_t num, bool endpoint);

    // -- Members

    const double start_;
    const double stop_;
    const size_t num_;
    const bool endpoint_;
};


}  // namespace eckit::geo::range
