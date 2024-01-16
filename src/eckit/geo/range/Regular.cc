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


#include "eckit/geo/range/Regular.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::range {


Regular::Regular(size_t n, double a, double b, double _eps) :
    Range(n, _eps), a_(a), b_(b) {
    if (types::is_approximately_equal(a, b, eps())) {
        endpoint_ = false;
    }
    else {
        b             = PointLonLat::normalise_angle_to_minimum(b, a + eps());
        auto inc      = (b - a) / static_cast<double>(n);
        auto periodic = types::is_approximately_greater_or_equal(b - a + inc, 360.);

        endpoint_ = !periodic;
    }
}


const std::vector<double>& Regular::values() const {
    if (values_.empty()) {
        auto& v = const_cast<std::vector<double>&>(values_);
        v       = util::linspace(0., 360., Range::size(), false);

        auto [from, to] = util::monotonic_crop(v, a_, b_, eps());
        v.erase(v.begin() + to, v.end());
        v.erase(v.begin(), v.begin() + from);

        ASSERT(!v.empty());
    }

    return values_;
}


}  // namespace eckit::geo::range
