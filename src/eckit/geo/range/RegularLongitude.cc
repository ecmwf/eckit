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


#include "eckit/geo/range/RegularLongitude.h"

#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


RegularLongitude::RegularLongitude(size_t n, double a, double b, double precision) :
    Range(n), a_(a), b_(b), precision_(precision) {
    if (types::is_approximately_equal(a, b, util::eps)) {
        endpoint_ = false;
    }
    else {

        b             = PointLonLat::normalise_angle_to_minimum(b, a + util::eps);
        auto inc      = (b - a) / static_cast<double>(n);
        auto periodic = types::is_approximately_greater_or_equal(b - a + inc, 360.);

        endpoint_ = !periodic;
    }
}


const std::vector<double>& RegularLongitude::values() const {
    if (empty()) {
        auto& v = const_cast<std::vector<double>&>(valuesVector());
        v       = util::linspace(0., 360., size(), false);

        auto [from, to] = util::monotonic_crop(v, a_, b_, 1e-3);
        v.erase(v.begin() + to, v.end());
        v.erase(v.begin(), v.begin() + from);
    }

    return *this;
}


}  // namespace eckit::geo::range
