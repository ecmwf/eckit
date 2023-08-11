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

#include "eckit/geometry/Iterator.h"
#include "eckit/geometry/Point.h"
#include "eckit/types/Fraction.h"


namespace eckit::geometry::iterator {


class Regular final : public Iterator {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Regular(size_t ni, size_t nj, double north, double west, double we, double ns);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    size_t ni_;
    size_t nj_;
    Fraction north_;
    Fraction west_;
    Fraction we_;
    Fraction ns_;
    size_t i_;
    size_t j_;
    double latValue_;
    double lonValue_;
    Fraction lat_;
    Fraction lon_;
    size_t count_;
    bool first_;

    Point p_;

    // -- Methods
    // None

    // -- Overridden methods

    bool operator!=(const Iterator&) override;
    bool operator++() override;
    bool operator--() override;

    explicit operator bool() override;
    Point& operator*() override;

    size_t size() const override;
    size_t index() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::iterator
