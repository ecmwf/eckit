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

    virtual const std::vector<double>& values() const = 0;

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

    explicit GaussianLatitude(size_t N, double eps = 0.) :
        GaussianLatitude(N, 90., -90., eps) {}

    GaussianLatitude(size_t N, double crop_a, double crop_b, double eps = 0.);

    // -- Methods

    size_t N() const { return N_; }

    // -- Overridden methods

    const std::vector<double>& values() const override;

private:
    // -- Members

    const size_t N_;
    std::vector<double> values_;
};


class RegularLongitude final : public Range {
public:
    // -- Constructors

    RegularLongitude(size_t n, double a, double b, double eps = 0.);
    RegularLongitude(size_t n, double a, double b, double crop_a, double crop_b, double eps = 0.);

    // -- Overridden methods

    const std::vector<double>& values() const override;

private:
    // -- Members

    bool periodic_;
    std::vector<double> values_;
};


}  // namespace range


}  // namespace eckit::geo
