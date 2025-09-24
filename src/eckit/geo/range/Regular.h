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


class Regular : public Range {
public:

    // -- Methods

    Fraction increment() const override;

    // -- Overridden methods

    const std::vector<double>& values() const override;

protected:

    // -- Constructors

    /**
     * @brief Regular
     * @param inc regular increment
     * @param a range start
     * @param b range end
     * @param ref User-defined coordinate reachable with (multiples of) integer increment; Can be defined out of the [a,
     * b] range. Support both "shifted" and "non-shifted" ranges, for the same definition of [a, b] range and increment
     */
    Regular(double inc, double a, double b, double ref);

    Regular(size_t n, double a, double b, bool periodic);

    Regular(size_t n, double a, double b, std::vector<double>&& values, bool periodic);

    Regular(const Regular&) = default;
    Regular(Regular&&)      = default;

    // -- Destructor

    ~Regular() override = default;

    // -- Operators

    Regular& operator=(Regular&&)      = default;
    Regular& operator=(const Regular&) = default;

    // -- Methods

    static Fraction adjust(const Fraction& target, const Fraction& inc, bool up);
    void increment(Fraction);

    void setPeriodic(bool p) { periodic_ = p; }
    bool getPeriodic() const { return periodic_; }

private:

    // -- Members

    Fraction increment_;
    std::vector<double> values_;
    bool periodic_;
};


class RegularCartesian final : public Regular {
public:

    // -- Constructors

    using Regular::Regular;

    RegularCartesian(size_t n, double a, double b);

    // -- Overridden methods

    [[nodiscard]] Range* make_range_cropped(double crop_a, double crop_b) const override;
};


class RegularLatitude final : public Regular {
public:

    // -- Constructors

    using Regular::Regular;

    explicit RegularLatitude(double inc, double a, double b, double ref);
    explicit RegularLatitude(size_t n, double a, double b);

    // -- Overridden methods

    [[nodiscard]] Range* make_range_cropped(double crop_a, double crop_b) const override;
};


class RegularLongitude final : public Regular {
public:

    // -- Constructors

    explicit RegularLongitude(double inc, double a, double b, double ref);
    explicit RegularLongitude(size_t n, double a, double b);

    // -- Overridden methods

    [[nodiscard]] Range* make_range_cropped(double crop_a, double crop_b) const override;

    bool periodic() const override { return getPeriodic(); }

private:

    // -- Constructors

    RegularLongitude(size_t n, double a, double b, std::vector<double>&& values, bool periodic) :
        Regular(n, a, b, std::move(values), periodic) {}
};


}  // namespace eckit::geo::range
