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

#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/spec/Custom.h"


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


void Composer::fill_spec(spec::Custom& custom) const {
    std::vector<std::string> specs;
    for (const auto* proj : *this) {
        specs.emplace_back(proj->spec_str());
    }

    custom.set("projections", specs);
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


Projection* Composer::compose_back(Projection* p, const Spec& spec) {
    return new Composer{p, ProjectionFactory::instance().get(spec.get_string("projection")).create(spec)};
}


Projection* Composer::compose_front(const Spec& spec, Projection* p) {
    return new Composer{ProjectionFactory::instance().get(spec.get_string("projection")).create(spec), p};
}


}  // namespace eckit::geo::projection
