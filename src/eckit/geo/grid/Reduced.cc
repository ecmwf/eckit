// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/Reduced.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Range.h"


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


const std::vector<size_t>& Reduced::nxacc() const {
    if (nxacc_.empty()) {
        nxacc_.resize(1 + ny());
        nxacc_.front() = 0;

        size_t j = 0;
        for (auto a = nxacc_.begin(), b = a + 1; b != nxacc_.end(); ++j, ++a, ++b) {
            *b = *a + nxj(j);
        }

        ASSERT(nxacc_.back() == size());
    }

    return nxacc_;
}


}  // namespace eckit::geo::grid
