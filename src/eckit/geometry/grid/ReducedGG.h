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

#include "eckit/geometry/grid/Gaussian.h"
#include "eckit/geometry/util.h"


namespace eckit::geometry::grid {


class ReducedGG : public Gaussian {
public:
    // -- Exceptions
    // None

    // -- Constructors

    explicit ReducedGG(const Configuration&);
    ReducedGG(size_t N, const pl_type& pl);

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

protected:
    // -- Constructors
    // None

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    bool isPeriodicWestEast() const override;

    // -- Class members

    // -- Class methods
    // None

private:
    // -- Members

    const pl_type& pl_;
    pl_type placc_;

    size_t k_;
    size_t Nj_;

    // -- Methods
    // None

    // -- Overridden methods

    size_t size() const override;

    std::vector<Point> to_points() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::grid
