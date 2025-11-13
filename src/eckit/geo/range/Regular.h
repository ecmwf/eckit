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

    // -- Constructors

    explicit Regular(double inc, double a, double b, double ref = 0);
    explicit Regular(Fraction inc, Fraction a, Fraction b);

    Regular(const Regular&) = default;
    Regular(Regular&&)      = default;

    // -- Operators

    Regular& operator=(const Regular&) = default;
    Regular& operator=(Regular&&)      = default;

    // -- Methods

    [[nodiscard]] static Regular* make_xy_range(double inc, double a, double b, double ref = 0);

    // -- Overridden methods

    [[nodiscard]] Regular* make_cropped_range(double crop_a, double crop_b) const override;

    Fraction increment() const override { return increment_; }
    const std::vector<double>& values() const override;

    // -- Methods

    Fraction af() const { return a_; }
    Fraction bf() const { return b_; }

    void af(Fraction value) {
        Range::a(value);
        a_ = Fraction{a()};
    }

    void bf(Fraction value) {
        Range::b(value);
        b_ = Fraction{b()};
    }

private:

    // -- Members

    Fraction increment_;
    Fraction a_;
    Fraction b_;
};


class RegularLatitudeRange : public LatitudeRange {
public:

    // -- Constructors

    RegularLatitudeRange(double inc, double a, double b, double ref = 0);

    // -- Overridden methods

    bool includesNorthPole() const override;
    bool includesSouthPole() const override;

    Fraction increment() const override { return regular_.increment(); }
    const std::vector<double>& values() const override { return regular_.values(); }

    [[nodiscard]] RegularLatitudeRange* make_cropped_range(double crop_a, double crop_b) const override;

private:

    // -- Constructors

    explicit RegularLatitudeRange(Regular&&);

    // -- Members

    Regular regular_;
};


class RegularLongitudeRange : public LongitudeRange {
public:

    // -- Constructors

    RegularLongitudeRange(double inc, double a, double b, double ref = 0);

    // -- Overridden methods

    bool periodic() const override { return periodic_; }

    Fraction increment() const override { return regular_.increment(); }
    const std::vector<double>& values() const override { return regular_.values(); }

    [[nodiscard]] RegularLongitudeRange* make_cropped_range(double crop_a, double crop_b) const override;

private:

    // -- Constructors

    explicit RegularLongitudeRange(Regular&&);

    // -- Members

    Regular regular_;
    bool periodic_;
};


}  // namespace eckit::geo::range
