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

    Range(const Range&) = delete;
    Range(Range&&)      = delete;

    // -- Destructors

    virtual ~Range() = default;

    // -- Operators

    Range& operator=(const Range&) = delete;
    Range& operator=(Range&&)      = delete;

    // -- Methods

    size_t size() const { return n_; }
    double a() const { return a_; }
    double b() const { return b_; }
    double eps() const { return eps_; }

    virtual bool periodic() const { return false; }

    [[nodiscard]] virtual Range* make_range_flipped() const                             = 0;
    [[nodiscard]] virtual Range* make_range_cropped(double crop_a, double crop_b) const = 0;

    virtual Fraction increment() const                = 0;
    virtual const std::vector<double>& values() const = 0;

protected:

    // -- Constructors

    explicit Range(size_t n, double a, double b, double eps = 0.);

    // --Methods

    void resize(size_t n);
    void a(double value) { a_ = value; }
    void b(double value) { b_ = value; }

private:

    // -- Members

    size_t n_;
    double a_;
    double b_;
    const double eps_;
};


}  // namespace eckit::geo
