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


#include "eckit/geo/range/LocalRegular.h"

#include "eckit/geo/util.h"


namespace eckit::geo::range {


LocalRegular::LocalRegular(size_t n, double a, double b, bool endpoint, double precision) :
    Range(n),
    a_(a),
    b_(b),
    endpoint_(endpoint),
    precision_(precision) {
}


const std::vector<double>& LocalRegular::values() const {
    if (empty()) {
        auto& v = const_cast<std::vector<double>&>(valuesVector());
        v       = util::linspace(a_, b_, size(), endpoint_);
    }

    return *this;
}


}  // namespace eckit::geo::range
