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
#include "eckit/geometry/area/BoundingBox.h"
#include "eckit/geometry/util.h"
#include "eckit/types/Fraction.h"


namespace eckit::geometry::iterator {


class ReducedIListJ : public Iterator {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit ReducedIListJ(const Grid&);

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

    const std::vector<double>& latitudes_;
    const pl_type pl_;
    const area::BoundingBox& bbox_;
    const size_t N_;
    size_t Ni_;
    size_t Nj_;
    Fraction lon_;
    double lat_;
    Fraction inc_;
    size_t i_;
    size_t j_;
    size_t k_;
    size_t count_;
    bool first_;
    Point p_;
    const size_t size_;

    // -- Methods

    size_t resetToRow(size_t);

    // -- Overridden methods

    bool operator==(const Iterator&) const override;
    bool operator++() override;
    bool operator+=(diff_t) override;

    explicit operator bool() const override;
    Point operator*() const override;

    size_t index() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::iterator
