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


Gaussian::Gaussian(size_t N) :
    Gaussian(N, 90., -90) {
}


Gaussian::Gaussian(size_t N, double a, double b, double precision) :
    Range(2 * N),
    N_(N),
    a_(a),
    b_(b),
    precision_(precision) {
    ASSERT(N > 0);
    ASSERT(precision_ >= 0.);
}


const std::vector<double>& Gaussian::values() const {
    if (empty()) {
        auto& v = const_cast<std::vector<double>&>(valuesVector());
        v       = util::gaussian_latitudes(N_, a_ < b_);
        ASSERT(v.size() == 2 * N_);

        const bool same(a_ == b_);

        auto a = a_;
        auto b = b_;

        if (a < v.back()) {
            a = v.back();
        }
        else {
            auto best = std::lower_bound(v.begin(), v.end(), a, [&](double l1, double l2) {
                return !types::is_approximately_equal(l1, l2, precision_) && !(l1 < l2);
            });
            ASSERT(best != v.end());
            a = *best;
        }

        if (same) {
            b = a;
        }
        else if (b > v.front()) {
            b = v.front();
        }
        else {
            auto best = std::lower_bound(v.rbegin(), v.rend(), b, [&](double l1, double l2) {
                return !types::is_approximately_equal(l1, l2, precision_) && !(l1 > l2);
            });
            ASSERT(best != v.rend());
            b = *best;
        }

        ASSERT((a_ <= b_) == (a <= b));
    }

    return *this;
}


}  // namespace eckit::geo::range
