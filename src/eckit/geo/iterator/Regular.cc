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
    // grid_(grid),
    x_(grid.x().values()),
    y_(grid.y().values()),
    i_(0),
    j_(0),
    index_(index),
    ni_(x_.size()),
    nj_(y_.size()),
    size_(ni_ * nj_) {}


bool Regular::operator==(const Iterator& other) const {
    const auto* another = dynamic_cast<const Regular*>(&other);
    return another != nullptr && index_ == another->index_;
}


bool Regular::operator++() {
    if (index_++, i_++; index_ < size_) {
        if (i_ >= ni_) {
            i_ = 0;
            j_++;
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
    return PointLonLat{x_.at(i_), y_.at(j_)};
}


void Regular::fill_spec(spec::Custom&) const {
    // FIXME implement
}


static const IteratorRegisterType<RegularInstance> ITERATOR_TYPE("regular");


}  // namespace eckit::geo::iterator
