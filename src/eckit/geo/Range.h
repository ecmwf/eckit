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
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Range(const Range&) = delete;
    Range(Range&&)      = delete;

    // -- Destructor

    virtual ~Range() = default;

    // -- Convertors
    // None

    // -- Operators

    Range& operator=(const Range&) = delete;
    Range& operator=(Range&&)      = delete;

    // -- Methods

    size_t size() const { return n_; }
    double eps() const { return eps_; }

    virtual const std::vector<double>& values() const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    explicit Range(size_t n, double eps = util::EPS);

    // -- Members
    // None

    // -- Methods

    void resize(size_t n) { n_ = n; }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    size_t n_;
    const double eps_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo
