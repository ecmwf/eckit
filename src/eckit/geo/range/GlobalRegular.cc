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


#include "eckit/geo/range/GlobalRegular.h"

#include "eckit/geo/util.h"


namespace eckit::geo::range {


GlobalRegular::GlobalRegular(size_t n, double a, double b, double precision) :
    Range(n),
    a_(a),
    b_(b),
    precision_(precision) {
}


const std::vector<double>& GlobalRegular::values() const {
    if (empty()) {
        auto& v = const_cast<std::vector<double>&>(valuesVector());
        v       = util::linspace(0., 360., size(), false);

        auto mm = util::monotonic_crop(v, a_, b_, 1e-3);
        v.erase(mm.second, v.end());
        v.erase(v.begin(), mm.first);
    }

    return *this;
}


}  // namespace eckit::geo::range
