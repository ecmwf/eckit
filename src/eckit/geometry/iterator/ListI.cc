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
    latitudes_(dynamic_cast<const grid::UnstructuredGrid&>(grid).latitudes_),
    longitudes_(dynamic_cast<const grid::UnstructuredGrid&>(grid).longitudes_),
    size_(grid.size()),
    index_(index) {
}


bool ListI::operator==(const Iterator& other) const {
    const auto* another = dynamic_cast<const ListI*>(&other);
    return another != nullptr && latitudes_.data() == another->latitudes_.data() && longitudes_.data() == another->longitudes_.data() && index_ == another->index_;
}


bool ListI::operator++() {
    index_++;
    return operator bool();
}


bool ListI::operator+=(diff_t d) {
    auto i = static_cast<diff_t>(index_);
    ASSERT(static_cast<size_t>(i) == index_);
    index_ = i + d < 0 ? size_ : static_cast<size_t>(i + d);
    return operator bool();
}


ListI::operator bool() const {
    return index_ < size_;
}


Point ListI::operator*() const {
    ASSERT(operator bool());
    return PointLonLat{longitudes_[index_], latitudes_[index_]};
}


size_t ListI::index() const {
    return index_;
}


}  // namespace eckit::geometry::iterator
