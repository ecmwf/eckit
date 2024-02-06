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
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


util::recursive_mutex MUTEX;


Range::Range(size_t n, double _a, double _b, double eps) :
    n_(n), a_(_a), b_(_b), eps_(eps) {
    ASSERT(n > 0);
    ASSERT(eps_ >= 0);
}


namespace range {


Regular::Regular(size_t n, double _a, double _b, double _eps) :
    Range(n, _a, types::is_approximately_equal(_a, _b, _eps) ? _a : _b, _eps), endpoint_(false) {
    constexpr auto db = 1e-12;

    // pre-calculate on n = 1
    if (a() < b()) {
        b(PointLonLat::normalise_angle_to_minimum(b() - db, a()) + db);
        auto inc  = (b() - a()) / static_cast<double>(size());
        endpoint_ = types::is_strictly_greater(360., b() - a() + inc);
    }
    else if (b() < a()) {
        b(PointLonLat::normalise_angle_to_maximum(b() + db, a()) - db);
        auto inc  = (a() - b()) / static_cast<double>(size());
        endpoint_ = types::is_strictly_greater(360., a() - b() + inc);
    }
    else {
        resize(1);
        endpoint_ = false;
        values_   = {a()};
    }
}


const std::vector<double>& Regular::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (values_.empty()) {
        const_cast<std::vector<double>&>(values_) = util::linspace(a(), b(), size(), endpoint_);
        ASSERT(!values_.empty());
    }

    return values_;
}

Range* Regular::crop(double a, double b) const {}


Gaussian::Gaussian(size_t N, double _a, double _b, double _eps) :
    Range(2 * N,
          types::is_approximately_equal(_a, 90., _eps)    ? 90.
          : types::is_approximately_equal(_a, -90., _eps) ? -90.
                                                          : _a,
          types::is_approximately_equal(_b, 90., _eps)    ? 90.
          : types::is_approximately_equal(_b, -90., _eps) ? -90.
                                                          : _b,
          _eps),
    N_(N) {
    // pre-calculate on cropping
    if (auto [min, max] = std::minmax(a(), b()); -90. < min || max < 90.) {
        values_ = util::gaussian_latitudes(N_, a() < b());
        auto& v = values_;

        auto [from, to] = util::monotonic_crop(v, min, max, eps());
        v.erase(v.begin() + to, v.end());
        v.erase(v.begin(), v.begin() + from);

        ASSERT(!v.empty());
        resize(v.size());
    }
}


const std::vector<double>& Gaussian::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    return values_.empty() ? util::gaussian_latitudes(N_, a() < b()) : values_;
}


}  // namespace range


}  // namespace eckit::geo
