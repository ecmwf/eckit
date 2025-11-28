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
