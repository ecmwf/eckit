/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <vector>

#include "eckit/geometry/Point2.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/value/Value.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit::geometry {

//----------------------------------------------------------------------------------------------------------------------

bool points_equal(const Point2& a, const Point2& b) {
    return eckit::types::is_approximately_equal<double>(Point2::distance2(a, b), 0.0);
}

Point2::operator Value() const {
    return Value::makeList(std::vector<Value>{x_[XX], x_[YY]});
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::geometry
