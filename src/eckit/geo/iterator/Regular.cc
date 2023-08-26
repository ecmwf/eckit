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

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/grid/Regular.h"


namespace eckit::geo::iterator {


Regular::Regular(const Grid& grid, size_t index) :
    geo::Iterator(grid),
    grid_(dynamic_cast<const grid::Regular&>(grid)),
    longitudes_(grid_.longitudes()),
    latitudes_(grid_.latitudes()),
    i_(0),
    j_(0),
    index_(index),
    ni_(grid_.ni()),
    nj_(grid_.nj()),
    index_size_(ni_ * nj_) {
    if (index_ < index_size_) {
        ASSERT(latitudes_.size() == grid_.nj());
    }
}


bool Regular::operator==(const Iterator& other) const {
    const auto* another = dynamic_cast<const Regular*>(&other);
    return another != nullptr && index_ == another->index_;
}


bool Regular::operator++() {
    if (index_++, i_++; index_ < index_size_) {
        if (i_ >= ni_) {
            i_ = 0;
            j_++;
        }

        return true;
    }

    index_ = index_size_;  // ensure it's invalid
    return false;
}


bool Regular::operator+=(diff_t d) {
    NOTIMP;
}


Regular::operator bool() const {
    return index_ < index_size_;
}


Point Regular::operator*() const {
    return PointLonLat{longitudes_.at(i_), latitudes_.at(j_)};
}


}  // namespace eckit::geo::iterator
