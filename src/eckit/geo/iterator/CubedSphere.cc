// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/iterator/CubedSphere.h"

#include "eckit/geo/grid/CubedSphere.h"


namespace eckit::geo::iterator {


CubedSphere::CubedSphere(const grid::CubedSphere& grid, size_t index) :
    grid_(grid), index_(index), size_(grid.size()), uid_(grid.uid()) {}


bool CubedSphere::operator==(const geo::Iterator& other) const {
    const auto* another = dynamic_cast<const CubedSphere*>(&other);
    return another != nullptr && index_ == another->index_ && uid_ == another->uid_;
}


bool CubedSphere::operator++() {
    if (index_++; index_ < size_) {
        return true;
    }

    index_ = size_;  // ensure it's invalid
    return false;
}


bool CubedSphere::operator+=(difference_type d) {
    if (auto di = static_cast<difference_type>(index_); 0 <= di + d && di + d < static_cast<difference_type>(size_)) {
        index_ = static_cast<size_t>(di + d);
        return true;
    }

    index_ = size_;  // ensure it's invalid
    return false;
}


CubedSphere::operator bool() const {
    return index_ < size_;
}


Point CubedSphere::operator*() const {
    return grid_.point(index_);
}


}  // namespace eckit::geo::iterator
