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

#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


Range::Range(size_t n, double _a, double _b, double _eps) : n_(n), a_(_a), b_(_b), eps_(_eps) {
    ASSERT(0 < n);
    ASSERT(0. <= eps_);
    if (types::is_approximately_equal(_a, _b)) {
        n_ = 1;
        b(_a);
    }
}


void Range::resize(size_t n) {
    ASSERT(0 < n);
    n_ = n;
}


}  // namespace eckit::geo
