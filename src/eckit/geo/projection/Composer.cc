// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geo/projection/Composer.h"

#include "eckit/spec/Custom.h"


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


const std::string& Composer::type() const {
    static const std::string type{"composer"};
    return type;
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
    return new Composer{p, ProjectionFactoryType::instance().get(spec.get_string("type")).create(spec)};
}


Projection* Composer::compose_front(const Spec& spec, Projection* p) {
    return new Composer{ProjectionFactoryType::instance().get(spec.get_string("type")).create(spec), p};
}


}  // namespace eckit::geo::projection
