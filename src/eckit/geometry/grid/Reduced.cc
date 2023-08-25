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


#include "eckit/geometry/grid/Reduced.h"

// #include <>


namespace eckit::geometry::grid {


size_t Reduced::size() const {
    return niacc().back();
}


Reduced::Reduced(const Configuration& config) :
    Grid(config) {
}


Reduced::Reduced(const area::BoundingBox& bbox) :
    Grid(bbox) {
}


const std::vector<size_t>& Reduced::niacc() const {
    if (niacc_.empty()) {
        niacc_.resize(1 + nj());
        niacc_.front() = 0;

        size_t j = 0;
        for (auto a = niacc_.begin(), b = a + 1; b != niacc_.end(); ++j, ++a, ++b) {
            *b = *a + ni(j);
        }

        ASSERT(niacc_.back() == size());
    }

    return niacc_;
}


}  // namespace eckit::geometry::grid
