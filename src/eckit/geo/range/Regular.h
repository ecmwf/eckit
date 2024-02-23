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


struct DiscreteRange {
    DiscreteRange(double a, double b, double inc, double ref);
    DiscreteRange(double a, double b, double inc, double ref, double period);

    Fraction a;
    Fraction b;
    Fraction inc;
    size_t n;
    bool periodic;
};


class Regular : public Range {
public:
    // -- Methods

    Fraction increment() const;
    bool periodic() const { return periodic_; }

    // -- Overridden methods

    const std::vector<double>& values() const override;

protected:
    // -- Constructors

    Regular(size_t n, double a, double b, double eps) :
        Range(n, a, b, eps), periodic_(false) {}

    Regular(size_t n, double a, double b, std::vector<double>&&, bool periodic, double eps);

    // -- Methods

    static Fraction adjust(const Fraction& target, const Fraction& inc, bool up);
    void periodic(bool p) { periodic_ = p; }

private:
    // -- Members

    std::vector<double> values_;
    bool periodic_;
};


}  // namespace eckit::geo::range
