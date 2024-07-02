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


Regular::Regular(const grid::Regular& grid, size_t index) :
    grid_(grid),
    x_(grid.x().values()),
    y_(grid.y().values()),
    i_(0),
    j_(0),
    index_(index),
    nx_(x_.size()),
    ny_(y_.size()),
    size_(nx_ * nx_) {}


bool Regular::operator==(const Iterator& other) const {
    const auto* another = dynamic_cast<const Regular*>(&other);
    return another != nullptr && index_ == another->index_;
}


bool Regular::operator++() {
    if (index_++, i_++; index_ < size_) {
        if (i_ >= nx_) {
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


}  // namespace eckit::geo::iterator
