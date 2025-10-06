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


#include "eckit/geo/iterator/Regular.h"

#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/grid/Regular.h"


namespace eckit::geo::iterator {


struct Instance {
    explicit Instance(const Spec& spec) : grid(dynamic_cast<const grid::Regular*>(GridFactory::build(spec))) {
        ASSERT(grid);
    }

    std::unique_ptr<const grid::Regular> grid;
};


struct RegularInstance : Instance, Regular {
    explicit RegularInstance(const Spec& spec) : Instance(spec), Regular(*grid) {}
};


Regular::Regular(const grid::Regular& grid, size_t index) :
    grid_(grid),
    projection_(grid_.projection()),
    x_(grid.x().values()),
    y_(grid.y().values()),
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
    NOTIMP;
}


Regular::operator bool() const {
    return index_ < size_;
}


Point Regular::operator*() const {
    return projection_.fwd(PointXY{x_.at(ix_), y_.at(iy_)});
}


static const IteratorRegisterType<RegularInstance> ITERATOR_TYPE("regular");


}  // namespace eckit::geo::iterator
