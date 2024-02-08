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


Range::Range(size_t n, double _a, double _b, double _eps) :
    n_(n), a_(_a), b_(_b), eps_(_eps) {
    ASSERT(n > 0);
    ASSERT(eps_ >= 0);
}


namespace range {


namespace {


util::recursive_mutex MUTEX;

constexpr auto DB = 1e-12;


double pole_snap(double lat, double _eps) {
    return types::is_approximately_equal(lat, 90., _eps)    ? 90.
           : types::is_approximately_equal(lat, -90., _eps) ? -90.
                                                            : lat;
}


}  // namespace


GaussianLatitude::GaussianLatitude(size_t N, double _a, double _b, double _eps) :
    Range(2 * N, pole_snap(_a, _eps), pole_snap(_b, _eps), _eps), N_(N) {
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


const std::vector<double>& GaussianLatitude::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    return values_.empty() ? util::gaussian_latitudes(N_, a() < b()) : values_;
}


RegularLongitude::RegularLongitude(size_t n, double _a, double _b, double _eps) :
    Range(n, _a, types::is_approximately_equal(_a, _b, _eps) ? _a : _b, _eps), endpoint_(false) {

    // pre-calculate on n = 1
    if (a() < b()) {
        b(PointLonLat::normalise_angle_to_minimum(b() - DB, a()) + DB);
        ASSERT(b() - a() <= 360.);

        auto inc  = (b() - a()) / static_cast<double>(size());
        endpoint_ = types::is_strictly_greater(360., b() - a() + inc);
    }
    else if (b() < a()) {
        b(PointLonLat::normalise_angle_to_maximum(b() + DB, a()) - DB);
        ASSERT(a() - b() <= 360.);

        auto inc  = (a() - b()) / static_cast<double>(size());
        endpoint_ = types::is_strictly_greater(360., a() - b() + inc);
    }
    else {
        resize(1);
        endpoint_ = false;
        values_   = {a()};
    }
}


const std::vector<double>& RegularLongitude::values() const {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (values_.empty()) {
        const_cast<std::vector<double>&>(values_) = util::linspace(a(), b(), size(), endpoint_);
        ASSERT(!values_.empty());
    }

    return values_;
}


RegularLongitude* RegularLongitude::crop(double _a, double _b) const {
    ASSERT_MSG((a() < b() && _a <= _b) || (b() < a() && _b <= _a), "Regular::crop: range does not respect ordering");

    NOTIMP;  // TODO
}


RegularLongitude RegularLongitude::make_global_prime(size_t n, double eps) {
    return {n, 0., 360., eps};
}


RegularLongitude RegularLongitude::make_global_antiprime(size_t n, double eps) {
    return {n, -180, 180., eps};
}


}  // namespace range


}  // namespace eckit::geo
