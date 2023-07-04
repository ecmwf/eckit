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


#include "eckit/geometry/Point.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"


namespace eckit::geometry {


bool points_equal(const Point& p, const Point& q) {
    ASSERT(p.index() == q.index());
    return std::visit([&](const auto& p, const auto& q) { return points_equal(p, q); }, p, q);
}


}  // namespace eckit::geometry


std::ostream& operator<<(std::ostream& out, const eckit::geometry::Point& p) {
    std::visit([&](const auto& p) { out << p; }, p);
    return out;
}
