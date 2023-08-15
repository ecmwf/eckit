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


#include "eckit/geometry/iterator/ListI.h"

#include "eckit/geometry/Grid.h"
#include "eckit/geometry/grid/UnstructuredGrid.h"


namespace eckit::geometry::iterator {


ListI::ListI(const Grid& grid, size_t index) :
    Iterator(grid),
    index_(index),
    points_(dynamic_cast<const grid::UnstructuredGrid&>(grid).points_),
    first_(true) {
}


bool ListI::operator==(const Iterator& other) {
    const auto* another = dynamic_cast<const ListI*>(&other);
    return another != nullptr && points_.data() == another->points_.data() && index_ == another->index_;
}


bool ListI::operator++() {
    if (first_) {
        first_ = false;
    }
    else {
        index_++;
    }
    return operator bool();
}


bool ListI::operator--() {
    if (index_ == 0) {
        return false;
    }
    index_--;
    return operator bool();
}


ListI::operator bool() {
    return index_ < points_.size();
}


const Point& ListI::operator*() {
    ASSERT(operator bool());
    return points_[index_];
}


size_t ListI::size() const {
    return points_.size();
}


size_t ListI::index() const {
    return index_;
}


}  // namespace eckit::geometry::iterator
