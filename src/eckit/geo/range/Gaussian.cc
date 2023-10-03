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


#include "eckit/geo/range/Gaussian.h"

#include <algorithm>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


Gaussian::Gaussian(size_t N, double a, double b, double precision) :
    Range(2 * N), N_(N), a_(a), b_(b), eps_(precision) {
    ASSERT(N > 0);
    ASSERT(eps_ >= 0.);

    // pre-calculate on cropping
    auto [min, max] = std::minmax(a_, b_);
    if (!types::is_approximately_equal(min, -90., eps_) || !types::is_approximately_equal(max, 90., eps_)) {
        auto& v = const_cast<std::vector<double>&>(values());

        auto [from, to] = util::monotonic_crop(v, min, max, precision);
        v.erase(v.begin() + to, v.end());
        v.erase(v.begin(), v.begin() + from);

        ASSERT(!empty());
    }
}


const std::vector<double>& Gaussian::values() const {
    if (empty()) {
        const_cast<std::vector<double>&>(valuesVector()) = util::gaussian_latitudes(N_, a_ < b_);
        ASSERT(!empty());
        ASSERT(size() == 2 * N_);
    }

    return *this;
}


}  // namespace eckit::geo::range
