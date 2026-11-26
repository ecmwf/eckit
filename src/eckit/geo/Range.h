// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/types/Fraction.h"


namespace eckit::geo {


class Range {
public:

    // -- Constructors

    Range() = default;

    Range(const Range&) = default;
    Range(Range&&)      = default;

    // -- Destructors

    virtual ~Range() = default;

    // -- Operators

    Range& operator=(const Range&) = default;
    Range& operator=(Range&&)      = default;

    // -- Methods

    double min() const;
    double max() const;

    [[nodiscard]] virtual Range* make_cropped_range(double crop_a, double crop_b) const = 0;
    [[nodiscard]] virtual const std::vector<double>& values() const                     = 0;

    virtual size_t size() const = 0;
    virtual double a() const    = 0;
    virtual double b() const    = 0;

    virtual Fraction increment() const;
    virtual bool periodic() const;
    virtual bool includesNorthPole() const;
    virtual bool includesSouthPole() const;
};


}  // namespace eckit::geo
