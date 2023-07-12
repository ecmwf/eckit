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

#include <memory>
#include <vector>

#include "eckit/geometry/Figure.h"
#include "eckit/geometry/Grid.h"
#include "eckit/geometry/Projection.h"


namespace eckit::geometry::detail {


class RegularGrid : public Grid {
public:
    // -- Types

    using LinearSpacing = std::vector<double>;

    // -- Exceptions
    // None

    // -- Constructors

    RegularGrid(const RegularGrid&) = delete;
    RegularGrid(RegularGrid&&)      = delete;

    // -- Destructor

    ~RegularGrid() override;

    // -- Convertors
    // None

    // -- Operators

    RegularGrid& operator=(const RegularGrid&) = delete;
    RegularGrid& operator=(RegularGrid&&)      = delete;

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

    RegularGrid(Projection*, const Configuration&);
    RegularGrid(Projection*, const area::BoundingBox&, const LinearSpacing& x, const LinearSpacing& y,
                const Figure&);

    // -- Members

    std::unique_ptr<Projection> projection_;
    LinearSpacing x_;
    LinearSpacing y_;
    Figure figure_;
    bool xPlus_;
    bool yPlus_;

    // -- Methods

    static Projection* make_projection_from_proj(const Configuration&);

    // -- Overridden methods

    bool includesNorthPole() const override;
    bool includesSouthPole() const override;
    bool isPeriodicWestEast() const override;

    void print(std::ostream&) const override;
    size_t size() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::detail
