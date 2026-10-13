// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/iterator/Regular.h"

#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Range.h"
#include "eckit/geo/grid/Regular.h"


namespace eckit::geo::iterator {


struct Instance {
    explicit Instance(const Grid::Spec& spec) : grid(dynamic_cast<const grid::Regular*>(GridFactory::build(spec))) {
        ASSERT(grid);
    }

    std::unique_ptr<const grid::Regular> grid;
};


struct RegularInstance : Instance, Regular {
    explicit RegularInstance(const Grid::Spec& spec) : Instance(spec), Regular(*grid) {}
};


Regular::Regular(const grid::Regular& grid, size_t index) :
    grid_(grid),
    xy_(grid.scan().is_scan_i_then_j()),
    alternating_(grid.scan().is_scan_alternating()),
    x_((xy_ ? grid.x() : grid.y()).values()),
    y_((xy_ ? grid.y() : grid.x()).values()),
    ix_(0),
    iy_(0),
    index_(index),
    nx_(x_.size()),
    ny_(y_.size()),
    size_(nx_ * ny_) {}


bool Regular::operator==(const Iterator& other) const {
    const auto* another = dynamic_cast<const Regular*>(&other);
    return another != nullptr && index_ == another->index_;
}


bool Regular::operator++() {
    if (index_++, ix_++; index_ < size_) {
        if (ix_ >= nx_) {
            ix_ = 0;
            iy_++;
        }

        return true;
    }

    index_ = size_;  // ensure it's invalid
    return false;
}


bool Regular::operator+=(difference_type d) {
    if (auto di = static_cast<difference_type>(index_); 0 <= di + d && di + d < static_cast<difference_type>(size_)) {
        index_ = static_cast<size_t>(di + d);
        ix_    = index_ % nx_;
        iy_    = index_ / nx_;
        return true;
    }

    index_ = size_;  // ensure it's invalid
    return false;
}


Regular::operator bool() const {
    return index_ < size_;
}


Point Regular::operator*() const {
    const auto ix = alternating_ && iy_ % 2 == 1 ? nx_ - 1 - ix_ : ix_;
    return xy_ ? grid_.point(x_.at(ix), y_.at(iy_)) : grid_.point(y_.at(iy_), x_.at(ix));
}


static const IteratorRegisterType<RegularInstance> ITERATOR_TYPE("regular");


}  // namespace eckit::geo::iterator
