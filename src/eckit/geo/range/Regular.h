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


namespace eckit::geo::range {


class Regular : public Range {
public:
    // -- Methods

    Fraction increment() const override;

    // -- Overridden methods

    const std::vector<double>& values() const override;

protected:
    // -- Constructors

    Regular(double inc, double a, double b, double ref, double eps);

    Regular(size_t n, double a, double b, bool periodic, double eps) : Range(n, a, b, eps), periodic_(periodic) {}

    Regular(size_t n, double a, double b, std::vector<double>&& values, bool periodic, double eps) :
        Range(n, a, b, eps), values_(values), periodic_(periodic) {}

    // -- Methods

    static Fraction adjust(const Fraction& target, const Fraction& inc, bool up);

    void setPeriodic(bool p) { periodic_ = p; }
    bool getPeriodic() const { return periodic_; }

private:
    // -- Members

    std::vector<double> values_;
    bool periodic_;
};


}  // namespace eckit::geo::range
