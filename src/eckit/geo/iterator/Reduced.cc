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


#include "eckit/geo/iterator/Reduced.h"

#include <algorithm>
#include <iterator>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/grid/Reduced.h"


namespace eckit::geo::iterator {


Reduced::Reduced(const Grid& grid, size_t index) :
    geo::Iterator(grid),
    grid_(dynamic_cast<const grid::Reduced&>(grid)),
    latitudes_(grid_.latitudes()),
    niacc_(grid_.niacc()),
    index_(index),
    index_size_(grid.size()) {
    if (index_ < index_size_) {
        longitudes_j_ = grid_.longitudes(j_ = j(index_));
        ASSERT(niacc_[j_] <= index && index_ < niacc_[j_ + 1]);
        ASSERT(latitudes_.size() == grid_.nj());
    }
}


bool Reduced::operator==(const Iterator& other) const {
    const auto* another = dynamic_cast<const Reduced*>(&other);
    return another != nullptr && index_ == another->index_;
}


bool Reduced::operator++() {
    if (index_++; index_ < index_size_) {
        if (!(index_ < niacc_[j_ + 1])) {
            longitudes_j_ = grid_.longitudes(++j_);
        }

        ASSERT(niacc_[j_] <= index_ && index_ < niacc_[j_ + 1]);
        return true;
    }

    index_ = index_size_;  // ensure it's invalid
    return false;
}


bool Reduced::operator+=(diff_t d) {
    if (auto di = static_cast<diff_t>(index_); 0 <= di + d && di + d < static_cast<diff_t>(index_size_)) {
        if (index_ = static_cast<size_t>(di + d); !(niacc_[j_] <= index_ && index_ < niacc_[j_ + 1])) {
            longitudes_j_ = grid_.longitudes(j_ = j(index_));
        }

        ASSERT(niacc_[j_] <= index_ && index_ < niacc_[j_ + 1]);
        return true;
    }

    index_ = index_size_;  // ensure it's invalid
    return false;
}


Reduced::operator bool() const {
    return index_ < index_size_;
}


Point Reduced::operator*() const {
    return PointLonLat{longitudes_j_.at(index_ - niacc_[j_]), latitudes_.at(j_)};
}


size_t Reduced::j(size_t idx) const {
    ASSERT(idx < index_size_);

    auto dist = std::distance(niacc_.begin(), std::upper_bound(niacc_.begin(), niacc_.end(), idx));
    ASSERT(1 <= dist && dist <= niacc_.size() - 1);

    return static_cast<size_t>(dist - 1);
}


}  // namespace eckit::geo::iterator
