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


#include "eckit/geo/projection/Composer.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::geo::projection {


std::vector<Point> Composer::fwd_points(const Point& p) const {
    if (empty()) {
        return {};
    }

    std::vector<Point> points;
    points.reserve(size());

    auto q = p;
    for (const auto* proj : *this) {
        points.emplace_back(proj->fwd(q));
        q = points.back();
    }

    return points;
}


std::vector<Point> Composer::inv_points(const Point& p) const {
    if (empty()) {
        return {};
    }

    std::vector<Point> points;
    points.reserve(size());

    auto q = p;
    for (auto proj = rbegin(); proj != rend(); ++proj) {
        points.emplace_back((*proj)->inv(q));
        q = points.back();
    }

    return points;
}


Spec* Composer::spec() const {
    NOTIMP;
}


Point Composer::fwd(const Point& p) const {
    auto q = p;
    for (const auto* proj : *this) {
        q = proj->fwd(q);
    }
    return q;
}


Point Composer::inv(const Point& p) const {
    auto q = p;
    for (auto proj = rbegin(); proj != rend(); ++proj) {
        q = (*proj)->inv(q);
    }
    return q;
}


}  // namespace eckit::geo::projection
