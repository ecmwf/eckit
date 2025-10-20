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


#include "eckit/geo/grid/Reduced.h"

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::grid {


Point Reduced::first_point() const {
    ASSERT(!empty());
    return PointLonLat{longitudes(0).front(), latitudes().front()};
}


Point Reduced::last_point() const {
    ASSERT(!empty());

    auto j = static_cast<int>(ny()) - 1;
    ASSERT(0 <= j);

    return PointLonLat{longitudes(j).back(), latitudes().back()};
}


Reduced::Reduced(const BoundingBox& bbox) : bbox_(bbox) {}


const std::vector<size_t>& Reduced::nxacc() const {
    if (nxacc_.empty()) {
        nxacc_.resize(1 + ny());
        nxacc_.front() = 0;

        size_t j = 0;
        for (auto a = nxacc_.begin(), b = a + 1; b != nxacc_.end(); ++j, ++a, ++b) {
            *b = *a + nx(j);
        }

        ASSERT(nxacc_.back() == size());
    }

    return nxacc_;
}


}  // namespace eckit::geo::grid
