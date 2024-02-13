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

    virtual Range* flip() const                             = 0;
    virtual Range* crop(double crop_a, double crop_b) const = 0;
    virtual const std::vector<double>& values() const       = 0;

protected:
    // -- Constructors

    explicit Range(size_t n, double a, double b, double eps = 0.);

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


namespace range {


class GaussianLatitude final : public Range {
public:
    // -- Constructors

    explicit GaussianLatitude(size_t N, bool increasing, double eps = 0.);

    // -- Methods

    size_t N() const { return N_; }

    // -- Overridden methods

    Range* flip() const override;
    Range* crop(double crop_a, double crop_b) const override;
    const std::vector<double>& values() const override;

private:
    // -- Constructors

    GaussianLatitude(size_t N, std::vector<double>&& values, double _eps) :
        Range(values.size(), values.front(), values.back(), _eps), N_(N), values_(values) {}

    // -- Members

    const size_t N_;
    std::vector<double> values_;
};


class RegularLongitude final : public Range {
public:
    // -- Constructors

    RegularLongitude(size_t n, double a, double b, double eps = 0.);

    // -- Methods
    bool periodic() const { return periodic_ != NonPeriodic; }

    // -- Overridden methods

    Range* flip() const override;
    Range* crop(double crop_a, double crop_b) const override;
    const std::vector<double>& values() const override;

private:
    // -- Types

    enum Periodic
    {
        NonPeriodic = 0,
        PeriodicNoEndPoint,
        PeriodicNoStartPoint
    };

    // -- Constructors

    RegularLongitude(size_t n, double a, double b, Periodic periodic, double eps) :
        Range(n, a, b, eps), periodic_(periodic) {}

    // -- Members

    Periodic periodic_;
    std::vector<double> values_;

    // -- Methods

    Fraction increment() const;
};


}  // namespace range


}  // namespace eckit::geo
