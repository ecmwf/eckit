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

#include "eckit/geo/util.h"


namespace eckit::geo::range {


Regular::Regular(size_t n, double a, double b, double precision) :
    Range(n), a_(a), b_(b), precision_(precision), endpoint_(false) {}


Regular::Regular(size_t n, double a, double b, bool endpoint, double precision) :
    Range(n), a_(a), b_(b), precision_(precision), endpoint_(endpoint) {}


const std::vector<double>& Regular::values() const {
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
