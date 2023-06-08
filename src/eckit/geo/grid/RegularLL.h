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

#include "eckit/geo/Grid.h"
#include "eckit/geo/Increments.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::grid {


class RegularLL : public Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    RegularLL(const Configuration&);
    RegularLL(const Increments&, const BoundingBox& = {}, const PointLonLat& reference = {0, 0});

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    static BoundingBox correctBoundingBox(const BoundingBox&, size_t& ni, size_t& nj, const Increments&,
                                          const PointLonLat& reference = {0, 0});

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const Increments increments_;
    const PointLonLat reference_;
    size_t ni_;
    size_t nj_;

    // -- Methods
    // None

    // -- Overridden methods

    void print(std::ostream&) const override;
    bool isPeriodicWestEast() const override;
    bool includesNorthPole() const override;
    bool includesSouthPole() const override;
    size_t numberOfPoints() const override;
    Iterator* iterator() const override;

    Renumber crop(BoundingBox&) const override;
    Renumber reorder(long scanningMode) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid
