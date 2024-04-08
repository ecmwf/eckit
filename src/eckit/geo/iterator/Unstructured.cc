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


#include "eckit/geo/iterator/Unstructured.h"

#include "eckit/geo/Grid.h"
#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::iterator {


Unstructured::Unstructured(const Grid& grid, size_t index) :
    index_(index),
    index_size_(grid.size()),
    longitudes_(dynamic_cast<const grid::Unstructured&>(grid).longitudes()),
    latitudes_(dynamic_cast<const grid::Unstructured&>(grid).latitudes()),
    uid_(dynamic_cast<const grid::Unstructured&>(grid).uid()) {
    ASSERT(index_size_ == longitudes_.size());
    ASSERT(index_size_ == latitudes_.size());
}


bool Unstructured::operator==(const geo::Iterator& other) const {
    const auto* another = dynamic_cast<const Unstructured*>(&other);
    return another != nullptr && index_ == another->index_ && uid_ == another->uid_;
}


bool Unstructured::operator++() {
    if (index_++; index_ < index_size_) {
        return true;
    }

    index_ = index_size_;  // ensure it's invalid
    return false;
}


bool Unstructured::operator+=(diff_t d) {
    if (auto di = static_cast<diff_t>(index_); 0 <= di + d && di + d < static_cast<diff_t>(index_size_)) {
        index_ = static_cast<size_t>(di + d);
        return true;
    }

    index_ = index_size_;  // ensure it's invalid
    return false;
}


Unstructured::operator bool() const {
    return index_ < index_size_;
}


Point Unstructured::operator*() const {
    return PointLonLat{longitudes_.at(index_), latitudes_.at(index_)};
}


}  // namespace eckit::geo::iterator
