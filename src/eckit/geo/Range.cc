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
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo {


Range::Range(size_t n, double _a, double _b, double _eps) :
    n_(n), a_(_a), b_(_b), eps_(_eps) {
    ASSERT(n > 0);
    ASSERT(eps_ >= 0);
    if (types::is_approximately_equal(_a, _b)) {
        n_ = 1;
        b(_a);
    }
}


void Range::resize(size_t n) {
    ASSERT(n > 0);
    n_ = n;
}


namespace range {}  // namespace range


}  // namespace eckit::geo
