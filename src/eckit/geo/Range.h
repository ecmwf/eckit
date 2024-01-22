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

#include "eckit/geo/util.h"


namespace eckit::geo {


class Range {
public:
    Range(const Range&) = delete;
    Range(Range&&)      = delete;

    virtual ~Range() = default;

    Range& operator=(const Range&) = delete;
    Range& operator=(Range&&)      = delete;

    size_t size() const { return n_; }
    double eps() const { return eps_; }

    virtual const std::vector<double>& values() const = 0;

protected:
    explicit Range(size_t n, double eps = util::EPS);

    void resize(size_t n) { n_ = n; }

private:
    size_t n_;
    const double eps_;
};


namespace range {


class Gaussian final : public Range {
public:
    explicit Gaussian(size_t N, double eps = 0.) :
        Gaussian(N, 90., -90., eps) {}

    Gaussian(size_t N, double crop_a, double crop_b, double eps = 0.);

    size_t N() const { return N_; }
    const std::vector<double>& values() const override;

private:
    const size_t N_;
    const double a_;
    const double b_;
    std::vector<double> values_;
};


class Regular final : public Range {
public:
    explicit Regular(size_t n, double a, double b, double eps = 0.);

    const std::vector<double>& values() const override;

private:
    double a_;
    double b_;
    bool endpoint_;
    std::vector<double> values_;
};


}  // namespace range


}  // namespace eckit::geo
