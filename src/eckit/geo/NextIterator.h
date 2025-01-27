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

#include "eckit/geo/Grid.h"


namespace eckit::geo {


class NextIterator final {
public:
    // -- Constructors

    explicit NextIterator(const Grid& grid) : NextIterator(grid.cbegin(), grid.cend()) {}

    // -- Methods

    bool next(Point&) const;
    size_t index() const { return index_; }

private:
    // -- Constructors

    NextIterator(Grid::Iterator&& current, Grid::Iterator&& end);

    // -- Members

    const std::unique_ptr<geo::Iterator> current_ptr_;
    const std::unique_ptr<const geo::Iterator> end_ptr_;
    geo::Iterator& current_;
    const geo::Iterator& end_;
    mutable size_t index_;
};


}  // namespace eckit::geo
