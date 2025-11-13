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


namespace eckit {
class Fraction;
}


namespace eckit::geo {


class Range {
public:

    // -- Constructors

    Range(const Range&) = default;
    Range(Range&&)      = default;

    // -- Destructors

    virtual ~Range() = default;

    // -- Operators

    Range& operator=(const Range&) = default;
    Range& operator=(Range&&)      = default;

    // -- Methods

    size_t size() const { return n_; }
    double a() const { return a_; }
    double b() const { return b_; }

    double min() const { return a_ < b_ ? a_ : b_; }
    double max() const { return a_ < b_ ? b_ : a_; }

    virtual Fraction increment() const                = 0;
    virtual const std::vector<double>& values() const = 0;

    [[nodiscard]] virtual Range* make_cropped_range(double crop_a, double crop_b) const = 0;

protected:

    // -- Constructors

    explicit Range(size_t n, double a, double b);

    // --Methods

    void resize(size_t n);
    void a(double value) { a_ = value; }
    void b(double value) { b_ = value; }

private:

    // -- Members

    size_t n_;
    double a_;
    double b_;
};


class LatitudeRange : public Range {
public:

    using Range::Range;
    using Range::operator=;

    virtual bool includesNorthPole() const = 0;
    virtual bool includesSouthPole() const = 0;
};


class LongitudeRange : public Range {
public:

    using Range::Range;
    using Range::operator=;

    virtual bool periodic() const = 0;
};


}  // namespace eckit::geo
