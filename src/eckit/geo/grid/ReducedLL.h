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


namespace eckit::geo::grid {


class ReducedLL : public Grid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    ReducedLL(const Configuration&);

    // -- Destructor

    ~ReducedLL() override;

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

    std::vector<long> pl_;

    // -- Methods
    // None

    // -- Overridden methods

    Iterator* iterator() const override;

    size_t numberOfPoints() const override;

    bool isPeriodicWestEast() const override;
    bool includesNorthPole() const override;
    bool includesSouthPole() const override;

    void print(std::ostream&) const override;

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
