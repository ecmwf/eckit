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

#include <utility>

#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/range/Regular.h"


namespace eckit::geo::range {


class RegularLongitude final : public Regular {
public:

    // -- Constructors

    explicit RegularLongitude(double inc, double a, double b, double ref);
    explicit RegularLongitude(size_t n, double a, double b);

    // -- Overridden methods

    [[nodiscard]] Range* make_range_cropped(double crop_a, double crop_b) const override;
    [[nodiscard]] Range* make_range_flipped() const override;

    bool periodic() const override { return getPeriodic(); }

private:

    // -- Constructors

    RegularLongitude(size_t n, double a, double b, std::vector<double>&& values, bool periodic) :
        Regular(n, a, b, std::move(values), periodic, PointLonLat::EPS) {}
};


}  // namespace eckit::geo::range
