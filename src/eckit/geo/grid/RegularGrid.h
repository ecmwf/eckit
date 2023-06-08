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

#include "eckit/geo/Grid.h"
#include "eckit/geo/Projection.h"


namespace eckit::geo {
class Shape {};
}  // namespace eckit::geo


namespace eckit::geo::grid {


class RegularGrid : public Grid {
public:
    // -- Types

    using LinearSpacing = std::vector<double>;

    // -- Exceptions
    // None

    // -- Constructors

    RegularGrid(const Configuration&, Projection*);
    RegularGrid(const Projection&, const BoundingBox&, const LinearSpacing& x, const LinearSpacing& y,
                const Shape&);
    RegularGrid(const RegularGrid&) = delete;

    // -- Destructor

    ~RegularGrid() override;

    // -- Convertors
    // None

    // -- Operators

    RegularGrid& operator=(const RegularGrid&) = delete;

    // -- Methods
    // None

    // -- Overridden methods

    Iterator* iterator() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members

    std::unique_ptr<Projection> projection_;
    LinearSpacing x_;
    LinearSpacing y_;
    Shape shape_;
    bool xPlus_;
    bool yPlus_;
    bool firstPointBottomLeft_;

    // -- Methods

    static Configuration make_proj_spec(const Configuration&);
    static LinearSpacing linspace(double start, double step, long num, bool plus);

    // -- Overridden methods

    bool includesNorthPole() const override;
    bool includesSouthPole() const override;
    bool isPeriodicWestEast() const override;

    void print(std::ostream&) const override;
    size_t numberOfPoints() const override;

    Renumber crop(BoundingBox&) const         override;
    Renumber reorder(long scanningMode) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid
