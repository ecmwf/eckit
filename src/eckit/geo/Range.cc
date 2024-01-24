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


#include "eckit/geo/Range.h"

#include <algorithm>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


Range::Range(size_t n, double eps) :
    n_(n), eps_(eps) {
    ASSERT(n > 0);
    ASSERT(eps_ >= 0);
}


namespace range {


Regular::Regular(size_t n, double a, double b, double _eps) :
    Range(n, _eps), a_(a), b_(b) {
    if (types::is_approximately_equal(a_, b_, eps())) {
        resize(1);
        b_      = a_;
        values_ = {a_};
    }
}


const std::vector<double>& Regular::values() const {
    if (values_.empty()) {
        auto& v = const_cast<std::vector<double>&>(values_);
        v       = util::linspace(a_, b_, size(), true);

        ASSERT(!v.empty());
    }

    return values_;
}


Gaussian::Gaussian(size_t N, double a, double b, double _eps) :
    Range(2 * N, _eps), N_(N), a_(a), b_(b) {
    ASSERT(N > 0);

    // pre-calculate on cropping
    auto [min, max] = std::minmax(a_, b_);
    if (!types::is_approximately_equal(min, -90., eps()) || !types::is_approximately_equal(max, 90., eps())) {
        values_ = util::gaussian_latitudes(N_, a_ < b_);
        auto& v = values_;

        auto [from, to] = util::monotonic_crop(v, min, max, eps());
        v.erase(v.begin() + to, v.end());
        v.erase(v.begin(), v.begin() + from);

        ASSERT(!v.empty());
        resize(v.size());
    }
}


const std::vector<double>& Gaussian::values() const {
    return values_.empty() ? util::gaussian_latitudes(N_, a_ < b_) : values_;
}


}  // namespace range


}  // namespace eckit::geo
